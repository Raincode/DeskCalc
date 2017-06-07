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
#include <cmath>
#include <complex>
#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <vector>

////////////////////////////////////////////////////////////////////////////////

namespace mps {
namespace str {
namespace {

////////////////////////////////////////////////////////////////////////////////

std::string replace_all(std::string str, const std::string& before, const std::string& after)
{
    std::size_t start_pos = 0;
    while((start_pos = str.find(before, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, before.length(), after);
        start_pos += after.length(); // Handles case where 'after' is a substring of 'before'
    }
    return str;
}

std::string replace_all(std::string s, char oldCh, char newCh)
{
    std::replace(begin(s), end(s), oldCh, newCh);
    return s;
}

////////////////////////////////////////////////////////////////////////////////

bool ends_with(const std::string& str, const std::string& tail)
{
    return str.size() >= tail.size()
        && str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
}

////////////////////////////////////////////////////////////////////////////////

bool starts_with(const std::string& str, const std::string& tail)
{
    return str.compare(0, tail.size(), tail) == 0;
}

////////////////////////////////////////////////////////////////////////////////

inline bool isvowel(char ch)
{
    ch = static_cast<char>(std::tolower(ch));
    return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u';
}

////////////////////////////////////////////////////////////////////////////////

constexpr unsigned char AE = static_cast<unsigned char>(142);
constexpr unsigned char ae = static_cast<unsigned char>(132);
constexpr unsigned char OE = static_cast<unsigned char>(153);
constexpr unsigned char oe = static_cast<unsigned char>(148);
constexpr unsigned char UE = static_cast<unsigned char>(154);
constexpr unsigned char ue = static_cast<unsigned char>(129);
constexpr unsigned char ss = static_cast<unsigned char>(225);

std::string ger(std::string str, char fmt='%')
{   // replaces all occurences of %aouAOU with äöüÄÖÜ
    std::size_t pos = 0;
    while ((pos = str.find(fmt)) != std::string::npos)
    {
        if (pos < str.size()-1)
        {
            char ch = str[pos + 1];
            switch (ch) {
            case 'a': ch = ae; break;
            case 'o': ch = oe; break;
            case 'u': ch = ue; break;
            case 'A': ch = AE; break;
            case 'O': ch = OE; break;
            case 'U': ch = UE; break;
            default:
                break;
            }
            //str.replace(pos, 2, ch);
        }
    }
    return str;
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

static const std::string ws{ " \t\r\n" };

std::string rtrim(const std::string& str)
{
    auto last = str.find_last_not_of(ws);
    return last == std::string::npos ? "" : str.substr(0, last + 1);
}

std::string ltrim(const std::string& str)
{
    auto first = str.find_first_not_of(ws);
    return first == std::string::npos ? "" : str.substr(first, str.size());
}

std::string trim(const std::string& str)
{
    return rtrim(ltrim(str));
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

std::string tolower(std::string s)
{
    std::transform(begin(s), end(s), begin(s), [](char c){return static_cast<char>(std::tolower(c));});
    return s;
}

////////////////////////////////////////////////////////////////////////////////

std::string toupper(std::string s)
{
    std::transform(begin(s), end(s), begin(s), [](char c){return static_cast<char>(std::toupper(c));});
    return s;
}

////////////////////////////////////////////////////////////////////////////////

static const std::vector<std::string> Apostrophies {
    "cant", "dont", "wont", "couldnt", "shouldnt", "wouldnt",
    "im", "its", "whats", "thats"
};

std::string correct_apostrophy(const std::string& str)
{
    std::string result;
    std::istringstream tokens{str};
    std::string delim{""};
    for (std::string word; tokens >> word; )
    {
        if (std::find(Apostrophies.begin(), Apostrophies.end(), tolower(word)) != Apostrophies.end())
            result += delim + word.replace(word.size() - 1, 0, "'");
        else if (word == "youre") result += "you're";
        else if (word == "Youre") result += "You're";
        else
            result += delim + word;
        delim = " ";
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

std::string strip(std::string str, const std::string& chars)
{   // removes all characters inside 'chars' from 'str'
    std::size_t pos{};
    while ((pos = str.find_first_of(chars)) != std::string::npos)
        str.erase(pos, 1);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

std::string strip_punct(std::string str)
{
    return strip(str, ".,!?");
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::string to_string(const T& value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

template<class T>
std::string to_string(const std::complex<T>& n)
{
    if (n.imag()) {
        std::string ret;
        if (n.real()) {
            ret += to_string(n.real());
            if (n.imag() > 0) ret += '+';
        }
        if (std::abs(n.imag()) != 1)
            ret += to_string(n.imag());
        return ret + 'i';
    }
    else
        return to_string(n.real());
}

////////////////////////////////////////////////////////////////////////////////

std::string format_number_EU(std::string num)
{
    num = replace_all(num, '.', ',');

    auto pos = num.find_first_of('e');
    if (pos != std::string::npos) {
        num.replace(pos, 1, "E");
        ++pos;

        auto plus = num.find_first_of("+", pos);
        if (plus != std::string::npos)
            num.erase(plus, 1);

        if (num[pos] == '-')
            ++pos;

        while (pos < num.size() && num[pos] == '0')
            num.erase(pos, 1);
    }
    return num;
}

template<class T>
std::string complex_to_str(const std::complex<T>& num)
{
    std::ostringstream os;
    if (num.imag()) {
        if (num.real()) os << num.real();
        if (num.real() && num.imag() > 0) os << '+';
        if (std::abs(num.imag()) != 1) os << num.imag();
        os << 'i';
    }
    else {
        os << num.real();
    }
    return os.str();
}

////////////////////////////////////////////////////////////////////////////////
}  // anonymous namespace
}  // namespace str
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
