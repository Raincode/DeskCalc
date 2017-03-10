////////////////////////////////////////////////////////////////////////////////
// DeskCalc is based on the calculator described in Bjarne Stroustrup's
//     "The C++ Programming Language"
//
// Copyright (C) 2017  Matthias Stauber
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "Parser.hpp"

#include <bitset>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "Function.hpp"

////////////////////////////////////////////////////////////////////////////////

#define MAKE_FUNC(func) [] (double arg) { return (func)(arg); }

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace math {
////////////////////////////////////////////////////////////////////////////////

Parser::Parser(TokenStream& tokstream, Context *context):
    ts{tokstream}, m_context{context}
{
    constexpr double pi = 3.1415926535897932385;

    m_context->funcTable["sqrt"] = MAKE_FUNC(std::sqrt);
    m_context->funcTable["cbrt"] = MAKE_FUNC(std::cbrt);

    m_context->funcTable["sin"] = MAKE_FUNC(std::sin);
    m_context->funcTable["cos"] = MAKE_FUNC(std::cos);
    m_context->funcTable["tan"] = MAKE_FUNC(std::tan);

    m_context->funcTable["asin"] = MAKE_FUNC(std::asin);
    m_context->funcTable["acos"] = MAKE_FUNC(std::acos);
    m_context->funcTable["atan"] = MAKE_FUNC(std::atan);

    m_context->funcTable["ln"] = MAKE_FUNC(std::log);
    m_context->funcTable["log"] = MAKE_FUNC(std::log10);

    m_context->funcTable["abs"] = MAKE_FUNC(std::abs);

    m_context->funcTable["round"] = MAKE_FUNC(std::round);
    m_context->funcTable["ceil"] = MAKE_FUNC(std::ceil);
    m_context->funcTable["floor"] = MAKE_FUNC(std::floor);
    m_context->funcTable["trunc"] = MAKE_FUNC(std::trunc);

    m_context->funcTable["CtoK"] = [] (double d) { return d - AbsoluteZero; };
    m_context->funcTable["KtoC"] = [] (double d) { return d + AbsoluteZero; };
    m_context->funcTable["CtoF"] = [] (double d) { return 1.8 * d + 32; };
    m_context->funcTable["FtoC"] = [] (double d) { return (d - 32) / 1.8; };

    m_context->funcTable["deg"] = [] (double rad) { return rad * 180 / pi; };
    m_context->funcTable["rad"] = [] (double deg) { return deg * pi / 180; };

    m_context->funcTable["hex"] = [this] (int num)
    {
        m_hasResult = false;

        if (num >= 0)
            std::cout << "0x" << std::hex << num << '\n';
        else
            std::cout << "hex only defined for natural numbers.\n";

        return 0;  // dummy value
    };

    m_context->funcTable["bin"] = [this] (int num)
    {
        m_hasResult = false;
        if (num >= 0)
        {
            std::ostringstream stream;
            stream << std::bitset<sizeof(int) * CHAR_BIT>(num);
            auto str = stream.str();
            auto begin = str.find_first_of("1");
            std::cout << (begin != std::string::npos ? str.substr(begin, str.size()) : "0") << '\n';
        }
        else
        {
            std::cout << "bin only defined for natural numbers.\n";
        }
        return 0;  // dummy value
    };

    init_vars();
}

void Parser::init_vars()
{
    m_context->varTable["pi"] = 3.1415926535897932385;
    m_context->varTable["e"] = 2.7182818284590452354;
}

/**
 * @brief Parses a single math statement.
 * @return Returns false if EOF is reached, else true.
 */
bool Parser::parse_stmt()
{
    if (ts.get().kind == Kind::End) return false;

    m_readNewline = ts.is_newline_print();
    m_hasResult = true;
    m_result = expr(false);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

double Parser::result() const
{
    assert(m_hasResult);
    return m_result;
}

////////////////////////////////////////////////////////////////////////////////

bool Parser::hasResult() const
{
    return m_hasResult;
}

////////////////////////////////////////////////////////////////////////////////

double Parser::get_var(const std::string& name) const
{
    if (!isvar(name))
    {
        error("Attempt to read variable " + name + " which is not defined.");
    }
    return m_context->varTable.find(name)->second;
}

////////////////////////////////////////////////////////////////////////////////

void Parser::set_var(const std::string& name, double val)
{
    m_context->varTable[name] = val;
}

////////////////////////////////////////////////////////////////////////////////

double Parser::expr(bool get)
{
    auto left = term(get);
    while (true)
    {
        switch (ts.current().kind) {
        case Kind::Plus:
            left += term(true);
            break;
        case Kind::Minus:
            left -= term(true);
            break;
        case Kind::RParen:
            if (!lparen_count)
            {
                error(ErrorType::UnexpectedToken);
            }
            --lparen_count;
        case Kind::Print:
        case Kind::End:
            return left;
        default:
            error("Syntax Error");
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

const Func& Parser::get_func(const std::string& name) const
{
    if (!isfunc(name))
    {
        throw std::runtime_error("Attempted to retrieve undefined function " + name);
    }
    return m_context->funcTable[name];
}

////////////////////////////////////////////////////////////////////////////////

struct Warning {
    Warning(const std::string& msg)
    {
        std::cout << msg;
    }
};

double Parser::term(bool get)
{
    auto left = pow(get);
    while (true)
    {
        switch (ts.current().kind) {
        case Kind::Mul:
            left *= pow(true);
            break;
        case Kind::Div:
            if (auto d = pow(true))
                left /= d;
            else error(ErrorType::DivideByZero);
            break;
        case Kind::Mod:
        {
            auto mod = pow(true);

            auto left_trunc = static_cast<long>(left);
            auto mod_trunc = static_cast<long>(mod);
            if (left - left_trunc || mod - mod_trunc)
            {
                throw std::runtime_error("Modulo not possible for floating point numbers");
            }

            left = static_cast<double>(left_trunc % mod_trunc);
            break;
        }
        case Kind::Parallel:
        {
            static Warning w{"Note: The precedence of the || operator is the same as * and /.\n"};
            auto right = term(true);
            if (left > 0 && right > 0)
            {
                // Rpar = R1 * R2 / (R1 + R2)
                return left * right / (left + right);
            }
            error("Resistors must be greater than 0");
        }
        default:
            return left;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

double Parser::pow(bool get)
{
    auto left = prim(get);
    while (true)
    {
        switch (ts.current().kind) {
        case Kind::Pow:
            return std::pow(left, pow(true));
        case Kind::Fac:  // experimental
            if (std::trunc(left) == left)
            {
                int max = std::trunc(left);
                for (int i = 2; i < max; ++i)
                {
                    left *= i;
                }
                return left;
            }
            error("! Operator only for integers.");
        default:
            return left;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Parser::func_def(bool get)
{
    expect(Kind::String, get);

    auto name = ts.current().strVal;
    if (isvar(name) || isfunc(name))
    {
        throw std::runtime_error(name + " is already defined");
    }

    expect(Kind::LParen, true);
    expect(Kind::String, true);

    auto var = ts.current().strVal;
    if (isset(var))
    {
        throw std::runtime_error("Cannot reuse already defined name as function variable");
    }

    expect(Kind::RParen, true);
    expect(Kind::Assign, true);

    ts.get();
    std::string term;
    while (ts.current().kind != Kind::Print &&
           ts.current().kind != Kind::End)
    {
        term += tokstr(ts.current());
        ts.get();
    }
    if (term.size())
    {
        m_context->funcTable[name] = Function(term, var, m_context);
    }
}

////////////////////////////////////////////////////////////////////////////////

void Parser::var_def(const std::string& name, bool get)
{
    if (get)
    {
        ts.get();
    }

    if (ts.current().kind != Kind::Assign)
    {
        //throw UndefinedNameError{name};
        error("'" + name + "' is not defined.");
    }

    m_context->varTable[name] = expr(true);
}

////////////////////////////////////////////////////////////////////////////////

double Parser::prim(bool get)
{
    if (get) ts.get();

    switch (ts.current().kind) {
    case Kind::Number:
    {
        auto val = ts.current().numVal;
        ts.get();
        return val;
    }
    case Kind::String:
    {
        auto name = ts.current().strVal;
        if (name == "fn")
        {
            m_hasResult = false;
            func_def(true);
            return 0;  // dummy value
        }
        else if (name == "dec")
        {
            expect(Kind::LParen, true);
            ++lparen_count;
            char ch{};
            if (!(ts.stream() >> ch))
                error("No Argument");
            int val{};
            if (ch == '0')  // bin or hex
            {
                ts.stream() >> ch;
                if (ch == 'x')
                    ts.stream() >> std::hex >> val >> std::dec;
                else
                {
                    std::string bin;
                    do {
                        bin += ch;
                        ts.stream().get(ch);
                    } while (ch == '0' || ch == '1');
                    ts.stream().unget();

                    for (std::size_t i = 0; i < bin.size(); ++i)
                        if (bin[i] != '0')
                            val += std::pow(2, bin.size() - i - 1);
                }
                ts.get();
            }
            else
            {
                ts.stream().unget();
                val = expr(true);
            }
            expect(Kind::RParen);
            ts.get();
            return val;
        }
        else if (isvar(name)) return resolve_var(name);
        else if (isfunc(name))  return exec_func(name);
        else
        {
           // m_hasResult = false;
            var_def(name, true);
            return m_context->varTable[name];
        }
    }
    // Allow application of mul/div to last result if lhs is omitted
    case Kind::Mul:
        if (isvar("Ans"))
            return get_var("Ans") * pow(true);
        error("Missing lhs operand");
    case Kind::Div:
        if (isvar("Ans"))
        {
            if (auto d = prim(true))
                return get_var("Ans") / d;
            else
                error(ErrorType::DivideByZero);
        }
        error("Missing lhs operand");
    case Kind::Minus:
        return -prim(true);
    case Kind::Plus:
        return prim(true);
    case Kind::LParen:
    {
        ++lparen_count;
        auto val = expr(true);
        expect(Kind::RParen);
        ts.get();
        return val;
    }
    default:
        error(ErrorType::UnexpectedToken);
    }
    throw std::logic_error("Parser::prim  fell through switch");  // silence warning
}

////////////////////////////////////////////////////////////////////////////////

double Parser::resolve_var(const std::string& name)
{
    if (ts.get().kind == Kind::Assign)
    {
        if (name == "e" || name == "pi")
        {
            error("Cannot overwrite nature constants.");
        }
       // m_hasResult = false;
        var_def(name, false);
       // return 0;  // dummy valuer
    }
    return m_context->varTable[name];
}

////////////////////////////////////////////////////////////////////////////////

double Parser::exec_func(const std::string& name)
{
    assert(isfunc(name));
    this_fn = name;
    expect(Kind::LParen, true);
    ++lparen_count;
    auto arg = expr(true);
    expect(Kind::RParen);
    ts.get();
    return m_context->funcTable[name](arg);
}

////////////////////////////////////////////////////////////////////////////////

void Parser::expect(Kind kind, bool get) const
{
    if (get) ts.get();

    if (ts.current().kind != kind)
        error("Expected Token " + tokstr(Token(kind)));
}

////////////////////////////////////////////////////////////////////////////////

void Parser::error(const std::string& msg) const
{
    lparen_count = {};
    throw std::runtime_error(msg);
}

////////////////////////////////////////////////////////////////////////////////

void Parser::error(ErrorType errorType) const
{
    switch (errorType) {
    case ErrorType::UndefinedName:
        error("Identifier '" + ts.current().strVal + "' is not defined.");
        break;
    case ErrorType::UnexpectedToken:
        error("Unexpected Token: " + tokstr(ts.current()) + ".");
        break;
    case ErrorType::FunctionRedefinition:
        error("Cannot redefine function (" + ts.current().strVal + ").");
    case ErrorType::DivideByZero:
        error("Divide by Zero");
    }
    throw std::logic_error("Parser::error: Unknown error type");  // silence warning
}

////////////////////////////////////////////////////////////////////////////////

bool Parser::isset(const std::string& name) const
{
    return isvar(name) || isfunc(name);
}

////////////////////////////////////////////////////////////////////////////////

bool Parser::isvar(const std::string& name) const
{
    return m_context->varTable.find(name) != m_context->varTable.end();
}

////////////////////////////////////////////////////////////////////////////////

bool Parser::isfunc(const std::string& name) const
{
    return m_context->funcTable.find(name) != m_context->funcTable.end();
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace math
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
