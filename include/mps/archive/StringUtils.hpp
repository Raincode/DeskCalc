////////////////////////////////////////////////////////////////////////////////
/// INSERT LICENSE HERE
////////////////////////////////////////////////////////////////////////////////

#ifndef MPS_UTIL_STRING_UTILS_HPP
#define MPS_UTIL_STRING_UTILS_HPP

////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cctype>
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////

namespace mps {
namespace util {

////////////////////////////////////////////////////////////////////////////////

static inline std::string replaceAll(std::string str, const std::string& before, const std::string& after)
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

namespace {
bool isvowel(char ch)
{
    ch = std::tolower(ch);
    return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u';
}
}

////////////////////////////////////////////////////////////////////////////////

const unsigned char AE = static_cast<unsigned char>(142);
const unsigned char ae = static_cast<unsigned char>(132);
const unsigned char OE = static_cast<unsigned char>(153);
const unsigned char oe = static_cast<unsigned char>(148);
const unsigned char UE = static_cast<unsigned char>(154);
const unsigned char ue = static_cast<unsigned char>(129);
const unsigned char ss = static_cast<unsigned char>(225);

static inline std::string ger(std::string str, char fmt='%')
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
            str.replace(pos, 2, { ch });
        }
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////

static inline unsigned word_count(const std::string& str,
                                  const std::function<bool(const std::string&)>& pred={})
{   // counts all words in str for which predicate applies
    std::istringstream stream {str};
    unsigned wc = 0;
    for (std::string word; stream >> word; )
        if (!pred || pred(word))  // simply count all words if no predicate is supplied
            ++wc;

    return wc;
}

////////////////////////////////////////////////////////////////////////////////

/*
static inline std::string read_alpha_str(std::istream& is, const std::string &extra_chars)
{   // reads characters until non-alpha character is encountered
    return read_alpha_str(is, [&extra_chars](char ch) {
        return extra_chars.find(ch) != std::string::npos;
    });
}
*/

////////////////////////////////////////////////////////////////////////////////

static inline std::string read_alpha_str(std::istream& is, const std::function<bool(char)>& pred)
{
    std::string str;
    char ch;
    do {  // ignore leading whitespace
        if (!is.get(ch)) return "";
    } while (std::isspace(ch));
    while (is && (std::isalpha(ch) || !pred || pred(ch)))
    {
        str += ch;
        is.get(ch);
    }
    is.putback(ch);
    return str;
}

////////////////////////////////////////////////////////////////////////////////
namespace {

std::string trim(const std::string& str)
{   // removes leading and trailing whitespace
    static const std::string ws{ " \t\r\n" };
    auto first = str.find_first_not_of(ws);
    auto last = str.find_last_not_of(ws);
    // handle whitespace only strings
    return first == std::string::npos ? "" : str.substr(first, last - first + 1);
}

////////////////////////////////////////////////////////////////////////////////


std::wstring wtrim(const std::wstring& str)
{   // removes leading and trailing whitespace
    static const std::wstring ws{ L" \t\r\n" };
    auto first = str.find_first_not_of(ws);
    auto last = str.find_last_not_of(ws);
    // handle whitespace only strings
    return first == std::wstring::npos ? L"" : str.substr(first, last - first + 1);
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

}  // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

static inline std::string tolower(std::string str)
{
    for (auto& ch : str)
        ch = std::tolower(ch);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

static inline std::string toupper(std::string str)
{
    for (auto& ch : str)
        ch = std::toupper(ch);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

static const std::vector<std::string> Apostrophies {
    "cant", "dont", "wont", "couldnt", "shouldnt", "wouldnt",
    "im", "its", "whats", "thats"
};

static inline std::string correct_apostrophy(const std::string& str)
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
static const std::map<std::string, std::string> Abbreviations {
    {"u", "you"}, {"<3", "love"}, {"dw", "don't worry"}, {"fml", "fuck my life"},
    {"np", "no problem"}, {"gj", "good job"}, {"gg", "good game"}, {"wp", "well played"},
    {"w/", "with"}, {"w/o", "without"}, {"plz", "please"}, {"u2", "you too"}, {"ty", "thank you"},
    {"cpy", "copy"}, {"ff", "surrender"}, {"mvp", "most valued player"}, {"bro", "brother"},
    {"ur", "you are"}
};

////////////////////////////////////////////////////////////////////////////////

static inline std::string strip_punctuation(std::string str)
{
    std::size_t pos{};
    while ((pos = str.find_first_of(".,!?")) != std::string::npos)
        str.erase(pos, 1);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

static inline std::string expand_abbrev(const std::string& str, const std::map<std::string, std::string>& abbrevMap=Abbreviations)
{
    std::string result;
    std::istringstream tokens{str};
    std::string delim {""};
    for (std::string word; tokens >> word; )
    {
        if (abbrevMap.find(word) != abbrevMap.end())
            result += delim + abbrevMap.find(word)->second;
        else
            result += delim + word;
        delim = " ";
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
static inline std::string to_string(const T& value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

////////////////////////////////////////////////////////////////////////////////

static inline std::string european_format(std::string number)
{
    std::size_t pos;
    if ((pos = number.find_first_of('.')) != std::string::npos)
    {
        number.replace(pos, 1, ",");
    }
    if ((pos = number.find_first_of('e')) != std::string::npos)
    {
        number.replace(pos, 1, "E");
    }
    if ((pos = number.find_first_of('+')) != std::string::npos ||
        (pos = number.find_first_of('-')) != std::string::npos)
    {
        number.erase(number.begin() + pos);
    }
    for ( ; pos < number.size() && number[pos] == '0'; )
    {
        number.erase(number.begin() + pos);
    }
    return number;
}

////////////////////////////////////////////////////////////////////////////////

}  // namespace util
}  // namespace mps

////////////////////////////////////////////////////////////////////////////////

#endif // MPS_UTIL_STRING_UTILS_HPP

////////////////////////////////////////////////////////////////////////////////
