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


#ifndef MPS_MATH_TOKEN_HPP
#define MPS_MATH_TOKEN_HPP

////////////////////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace mps {
namespace math {

////////////////////////////////////////////////////////////////////////////////

enum class Kind : char {
    Number, String, Print, End,
    Plus='+', Minus='-', Mul='*', Div='/', Mod='%',
    Pow='^', Fac='!', LParen='(', RParen=')', Assign='=',
    Parallel, /* Range, */ Invalid
};

////////////////////////////////////////////////////////////////////////////////

struct Token {
    explicit Token(Kind k)
        :kind{k} { }
    Token(Kind k, const std::string& str)
        :kind{k}, strVal{str} { }
    Token(Kind k, double num)
        :kind{k}, numVal{num} { }

    Kind kind;
    double numVal{};
    std::string strVal;
};

////////////////////////////////////////////////////////////////////////////////

inline std::string tokstr(Token t)
{
    switch (t.kind) {
    case Kind::Number:
        return std::to_string(t.numVal);
    case Kind::String:
        return t.strVal;
    case Kind::Print:
        return "Print";
    case Kind::End:
        return "End";
    default:
        return { static_cast<char>(t.kind) };
    }
}

////////////////////////////////////////////////////////////////////////////////

}  // namespace math
}  // namespace mps

////////////////////////////////////////////////////////////////////////////////

#endif // MPS_MATH_TOKEN_HPP

////////////////////////////////////////////////////////////////////////////////
