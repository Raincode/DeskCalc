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

#include <iostream>
#include <fstream>
#include <string>

#include "Parser.hpp"
#include "TokenStream.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace math {
////////////////////////////////////////////////////////////////////////////////

class Calculator {
public:
    Calculator() = default;

    void run(std::istream& is, std::ostream& os=std::cout);
    void run_cli(); // cli interaction is different from file etc.
    void exec(const std::string& arg);

    void show_intro() const;

private:
    bool process_command(const std::string& input);

private:
    TokenStream mTokenStream;
    Context mParserContext;
    Parser mParser {mTokenStream, &mParserContext};
    bool mIsRunning {true};
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace math
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
