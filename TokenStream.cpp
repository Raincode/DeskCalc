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

#include "TokenStream.hpp"

#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace math {
////////////////////////////////////////////////////////////////////////////////

TokenStream::TokenStream(std::istream& is):
    input{&is}
{
}

////////////////////////////////////////////////////////////////////////////////

TokenStream::TokenStream(std::istream *is):
    input{is}, ownsInput{true}
{
}

////////////////////////////////////////////////////////////////////////////////

TokenStream::TokenStream(const std::string& str):
    input{new std::istringstream(str)}, ownsInput{true}
{
}

////////////////////////////////////////////////////////////////////////////////

TokenStream::~TokenStream()
{
    cleanup();
}

////////////////////////////////////////////////////////////////////////////////

void TokenStream::set_input(std::istream& is)
{
    cleanup();
    input = &is;
    ownsInput  = false;
}

////////////////////////////////////////////////////////////////////////////////

void TokenStream::set_input(std::istream *is)
{
    cleanup();
    input = is;
    ownsInput = true;
}

////////////////////////////////////////////////////////////////////////////////

void TokenStream::set_input(const std::string& str)
{
    set_input(new std::istringstream(str));
}

////////////////////////////////////////////////////////////////////////////////

Token TokenStream::get()
{
    char ch;
    do {
        if (!input->get(ch))
            return ct = Token(Kind::End);
    } while (std::isspace(ch) && ch != '\n');

    switch (ch) {
    case ';':
    case '\n':
        newlineRead = (ch == '\n');
        return ct = Token(Kind::Print);
    case '+':
    case '-':
    case '/':
    case '%':
    case '^':
    case '!':
    case '(':
    case ')':
    case '=':
        return ct = Token(static_cast<Kind>(ch));
    case '*':
        // allow ** as well as ^ as Pow operators
        return ct = parse_double_op('*', Kind::Pow, Kind::Mul);
    case '|':
        return ct = parse_double_op('|', Kind::Parallel, Kind::Invalid);
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case '.':
        input->putback(ch);
        *input >> ct.numVal;
        ct.kind = Kind::Number;
        return ct;
    default:
        return ct = parse_identifier(ch);
    }
    std::ostringstream os;
    os << "Bad Token: " << ch;
    // skip all tokens in current statement
    while (ct.kind != Kind::Print) get();
    throw std::runtime_error(os.str());
}

////////////////////////////////////////////////////////////////////////////////

Token TokenStream::parse_identifier(char ch)
{
    if (std::isalpha(ch) || ch == '_')
    {
        std::string str {ch};
        while (input->get(ch) && (std::isalnum(ch) || ch =='_'))
        {
             str += ch;
        }
        input->putback(ch);
        return {Kind::String, str};
    }
    else
    {
        throw std::runtime_error("Bad Token");
    }
}

////////////////////////////////////////////////////////////////////////////////

Token TokenStream::parse_double_op(char second, Kind atMatch, Kind firstKind)
{
    char ch;
    if (input->get(ch) && ch == second)
    {
        return Token(atMatch);
    }
    else
    {
        input->putback(ch);
        if (firstKind != Kind::Invalid)
        {
            return Token(firstKind);
        }
        else
        {
            throw std::runtime_error("Bad Token");
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

bool TokenStream::is_newline_print() const
{
    return ct.kind == Kind::Print && newlineRead;
}

////////////////////////////////////////////////////////////////////////////////

const Token& TokenStream::current() const
{
    return ct;
}

////////////////////////////////////////////////////////////////////////////////

void TokenStream::cleanup()
{
    if (ownsInput) delete input;
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace math
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
