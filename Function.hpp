////////////////////////////////////////////////////////////////////////////////
// DeskCalc is based on the calculator described in Bjarne Stroustrup's
//     "The C++ Programming Language"
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

#include <string>

#include "Parser.hpp"
#include "strutil.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace math {
////////////////////////////////////////////////////////////////////////////////

class Function {
public:
    Function(const std::string& t /* term */, const std::string& v /* var */, Context *c)
        :term{t}, var{v}, context{c} { }

    double operator()(double arg) const
    {
        auto expr = util::replaceAll(term, var, std::to_string(arg));
        TokenStream str(expr);
        static Parser p(str, context);
        return p.expr(true);
    }

private:
    std::string term;
    std::string var;
    Context *context;
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace math
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
