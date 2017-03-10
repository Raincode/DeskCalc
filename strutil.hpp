////////////////////////////////////////////////////////////////////////////////
// Provides std::string utility functions
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
#include <functional>
#include <string>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace util {
namespace {
////////////////////////////////////////////////////////////////////////////////

std::string replaceAll(std::string str, const std::string& before, const std::string& after)
{
    size_t start_pos = 0;
    while((start_pos = str.find(before, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, before.length(), after);
        start_pos += after.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////

inline bool isvowel(char ch)
{
    ch = std::tolower(ch);
    return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u';
}

////////////////////////////////////////////////////////////////////////////////

unsigned word_count(const std::string& str,
                    const std::function<bool(const std::string&)>& pred={})
{   // counts all words in str (for which optional predicate applies)
    std::istringstream stream {str};
    unsigned wc = 0;
    for (std::string word; stream >> word ;)
        if (!pred || pred(word))
            ++wc;
    return wc;
}

////////////////////////////////////////////////////////////////////////////////

std::string read_alpha_str(std::istream& is, const std::function<bool(char)>& pred)
{
    std::string str;
    char ch;
    do {  // ignore leading whitespace
        if (!is.get(ch)) return "";
    } while (std::isspace(ch));
    while (is &&( std::isalpha(ch) || (pred && pred(ch)) ))
    {
        str += ch;
        is.get(ch);
    }
    is.putback(ch);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

std::string trim(const std::string& str)
{   // removes leading and trailing whitespace
    static const std::string ws{ " \t\r\n" };
    auto first = str.find_first_not_of(ws);
    auto last = str.find_last_not_of(ws);
    // handle pure whitespace
    return first == std::string::npos ? "" : str.substr(first, last - first + 1);
}

////////////////////////////////////////////////////////////////////////////////

std::string single_ws(const std::string& str)
{   // convert all adjacent whitespace to single spaces
    std::string result;
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        if (i && std::isspace(str[i]))
        {   // don't insert more than one space inbetween characters
            if (!std::isspace(str[i - 1]))
                result += ' ';
        }
        else result += str[i];
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string tolower(std::string str)
{
    for (auto& ch : str)
        ch = std::tolower(ch);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

std::string toupper(std::string str)
{
    for (auto& ch : str)
        ch = std::toupper(ch);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

std::string strip_punctuation(std::string str)
{
    std::size_t pos{};
    while ((pos = str.find_first_of(".,!?")) != std::string::npos)
        str.erase(pos, 1);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::string to_string(const T& value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

////////////////////////////////////////////////////////////////////////////////

std::string european_format(std::string number)
{
    auto end = std::string::npos;

    std::size_t pos = number.find_first_of('.');
    if (pos != end) number.replace(pos, 1, ",");

    pos = number.find_first_of('e');
    if (pos != end)
    {
        number.replace(pos, 1, "E");
        ++pos;
        std::size_t sign;
        if ((sign = number.find_first_of("+", pos)) != end)
        {
            number.erase(number.begin() + sign);
        }
        else ++pos;
        while (pos < number.size() && number[pos] == '0')
        {
            number.erase(number.begin() + pos);
        }
    }
    return number;
}

////////////////////////////////////////////////////////////////////////////////
}  // anonymous namespace
}  // namespace util
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
