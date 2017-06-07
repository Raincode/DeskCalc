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

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>

#include "str_util.hpp"

#undef max

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace {
////////////////////////////////////////////////////////////////////////////////

bool parse_int(const std::string& str, int& out)
{
    if (str.empty() || str.front() == '0') return false;
    for (auto c : str)
        if (!std::isdigit(c)) return false;

    out = std::stoi(str);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
T get_num(const std::string& prompt)
{   // Gets a number of type T from std::cin, rejecting input which is nothing but a number
    std::cout << prompt;
    std::string input;

    while (!str::trim(input).size())  // skip empty input lines
        if (!std::getline(std::cin, input))  // EOF or cin.bad()
            throw std::runtime_error("No Input");

    std::istringstream stream{ input };
    T n;
    if (stream >> n)
    {   // test for remaining characters
        char ch;
        if (!(stream >> ch)) return n;
    }
    std::cout << "Format Error.\n";
    return get_num<T>(prompt);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
T get_num(const std::string& prompt, T min, T max)
{   // get_num only accepting numbers in range [min;max]
    T n = get_num<T>(prompt);
    while (n < min || n > max)
    {
        std::cerr << "Not in range [" << min << ';' << max << "]\n";
        n = get_num<T>(prompt);
    }
    return n;
}

std::string get_str(const std::string& prompt)  // in future maybe add format?
{
    std::cout << prompt;
    std::string ret;
    if (!(std::cin >> ret)) {
        std::cin.clear();
        throw std::runtime_error{ "No Input" };
    }
    return ret;
}

void recover_line(std::istream& is)
{
    is.clear();
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool ask_user(const std::string& question)
{
    std::cout << question << " (Y/n)? ";
    char c{};
    do {
        if (!(std::cin >> c))
            return false;
        if (c == 'y')
            std::cout << "Please enter 'Y' instead of 'y': ";
    } while (c == 'y');
    std::cin.ignore();  // discard remaining '\n'
    return c == 'Y';
}

void print_binary(std::ostream& os, int n)
{
    if (n <= 1) {
        os << n;
        return;
    }
    print_binary(os, n >> 1);
    os << n % 2;
}

bool read_hex(std::istream& is, int& out)
{
    char c1, c2;
    if (!is.get(c1))
        return false;

    if (!is.get(c2)) {
        is.putback(c1);
        return false;
    }

    if (c1 == '0' && c2 == 'x')
        return (is >> std::hex >> out >> std::dec).good();

    is.putback(c2);
    is.putback(c1);
    return false;
}

bool read_bin(std::istream& is, int& out)
{
    char c;
    if (!is.get(c))
        return false;

    if (c == '0') {
        for (out = 0; is.get(c) && c == '0' || c == '1'; ) {
            out <<= 1;
            if (c == '1')
                ++out;
        }
        return true;
    }

    is.putback(c);
    return false;
}

////////////////////////////////////////////////////////////////////////////////

static inline void keep_window_open()
{
    std::cout << "Press <RETURN> to close this window...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

////////////////////////////////////////////////////////////////////////////////
} // anonymous namespace
} // namespace mps
////////////////////////////////////////////////////////////////////////////////
