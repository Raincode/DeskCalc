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

#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <sstream>

#include "Calculator.hpp"

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
try
{
    mps::math::Calculator calc;
    switch (argc) {
    case 3:
    {
        std::ifstream is {argv[1]};
        std::ofstream os {argv[2]};
        if (!is)
            std::cerr << "Unable to open input file '" << argv[1] << "'\n";
        else if (!os)
            std::cerr << "Unable to open output file '" << argv[2] << "'\n";
        else
        {
            calc.run(is, os);
            return 0;
        }
        return -1;
    }
    case 2:
        if (argv[1] != std::string("-"))
        {   // try to open argument as input file, else try to run as expression
            std::ifstream is {argv[1]};
            (is) ? calc.run(is) : calc.exec(argv[1]);
            return 0;
        } // fall through case label: argument "-" means use stdin;
    case 1:
        calc.show_intro();
        calc.run_cli();
        return 0;
    default:
        std::cerr << "Too many arguments.\n"
                     "Usage:    dcalc [infile [outfile] | stmt-list]\n";
        return 1;
    }
}
catch (std::exception& e)
{
    std::cerr << ' ' << e.what() << '\n';
    return 2;
}
catch (...)
{
    std::cerr << " An unknown error has occured.\n";
    return 3;
}

////////////////////////////////////////////////////////////////////////////////
