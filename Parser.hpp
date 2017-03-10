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
#pragma once
////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Node.hpp"
#include "TokenStream.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace mps {
namespace math {

////////////////////////////////////////////////////////////////////////////////

using Func = std::function<double(double)>;

////////////////////////////////////////////////////////////////////////////////

class Parser;

struct Context {
    std::map<std::string, Func> funcTable;
    std::map<std::string, double> varTable;
};

////////////////////////////////////////////////////////////////////////////////

class Parser {
public:
    struct UndefinedNameError {
        std::string name;
    };

    Parser(TokenStream& tokenStream, Context *context);

    bool parse_stmt();
    void func_def(bool get);
    void var_def(const std::string& name, bool get);
    double expr(bool get);
    double term(bool get);
    double pow(bool get);
    double prim(bool get);

    bool hasResult() const;
    double result() const;

    void set_var(const std::string& name, double val);
    double get_var(const std::string& name) const;
    const std::map<std::string, double>& get_vars() const { return m_context->varTable; }
    void clear_vars() {
        m_context->varTable.clear();
        init_vars();
    }

    const Func& get_func(const std::string& name) const;

    bool read_newline() const { return m_readNewline; }

    TokenStream& token_stream() { return ts; }

    bool isset(const std::string& name) const;
    bool isvar(const std::string& name) const;
    bool isfunc(const std::string& name) const;

    Context* context() const { return m_context; }

private:
    enum class ErrorType
    {
        DivideByZero,
        FunctionRedefinition,
        UndefinedName,
        UnexpectedToken
    };

    void error(const std::string& msg) const;
    void error(ErrorType errorType) const;
    void expect(Kind kind, bool get=false) const;
    double exec_func(const std::string& name);
    double resolve_var(const std::string& name);
    void init_vars();

////////////////////////////////////////////////////////////////////////////////

    static constexpr double AbsoluteZero {-273.15}; // In Celsius

    TokenStream& ts;

    std::string this_fn;

    bool m_readNewline {false};

    // Funtion or Variable definitions don't yield a result
    bool m_hasResult {false};
    double m_result {};

    // # of open parenthesis while parsing
    mutable int lparen_count {}; // mutable for error cleanup

    Context *m_context;
};

////////////////////////////////////////////////////////////////////////////////

}  // namespace math
}  // namespace mps

////////////////////////////////////////////////////////////////////////////////
