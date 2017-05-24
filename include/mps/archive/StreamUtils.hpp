////////////////////////////////////////////////////////////////////////////////
/// INSERT LICENSE HERE
////////////////////////////////////////////////////////////////////////////////

#ifndef MPS_UTIL_STREAMUTILS_HPP
#define MPS_UTIL_STREAMUTILS_HPP

////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <limits>
#include <string>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////

namespace mps {
namespace util {

////////////////////////////////////////////////////////////////////////////////

static inline std::string trim(const std::string& str)
{   // removes leading and trailing whitespace
    static const std::string ws{ " \t\r\n" };
    auto first = str.find_first_not_of(ws);
    auto last = str.find_last_not_of(ws);
    // handle whitespace only strings
    return first == std::string::npos ? "" : str.substr(first, last - first + 1);
}


////////////////////////////////////////////////////////////////////////////////

static inline void cls()
{   // unix: std::cout << "\x1B[2J\x1B[H"; ?
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO buf_info{};
    if (!GetConsoleScreenBufferInfo(hConsole, &buf_info)) return;

    DWORD conSize{static_cast<DWORD>(buf_info.dwSize.X * buf_info.dwSize.Y)};
    DWORD charsWritten{};
    COORD coordScreen{0, 0};
    if (!FillConsoleOutputCharacter(hConsole, ' ', conSize, coordScreen, &charsWritten)) return;

    if (!GetConsoleScreenBufferInfo(hConsole, &buf_info)) return;
    if (!FillConsoleOutputAttribute(hConsole, buf_info.wAttributes, conSize, coordScreen, &charsWritten)) return;
    SetConsoleCursorPosition(hConsole, coordScreen);
}

////////////////////////////////////////////////////////////////////////////////

static inline bool get_str(std::istream& is, std::string& str)
{
    if (!(is >> str))
    {
        is.clear();
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

namespace {
    template<typename T>
    bool parse_num(const std::string &input, T &num)
    {
        std::istringstream stream{input};
        T n;
        if (stream >> n)
        {
            char ch;
            if (!(stream >> ch))
            {
                num = n;
                return true;
            }
        }
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
static inline T get_num(const std::string& prompt)
{   // Gets a number of type T from std::cin, rejecting input which is nothing but a number
    std::cout << prompt;
    std::string input;

    while (!mps::util::trim(input).size())  // skip empty input lines
        if (!std::getline(std::cin, input))  // EOF or cin.bad()
            throw std::runtime_error("No Input");

    T n;
    if (parse_num(input, n)) return n;
    std::cout << "Format Error.\n";
    return get_num<T>(prompt);
}
////////////////////////////////////////////////////////////////////////////////

template<typename T>
static inline bool get_num(T& num, const std::string& prompt)
{
    try {
        num = get_num<T>(prompt);
    }
    catch (std::runtime_error &e) {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
static inline bool get_num(T& num, const std::string& prompt, T min, T max)
{
    try {
        num = get_num<T>(prompt);
        while (num < min || num > max)
        {
            std::cerr << "Not in range [" << min << ';' << max << "]\n";
            num = get_num<T>(prompt);
        }
    }
    catch (std::runtime_error &e) {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
static inline T get_num(const std::string& prompt, T min, T max)
{   // get_num only accepting numbers in range [min;max]
    T n = get_num<T>(prompt);
    while (n < min || n > max)
    {
        std::cerr << "Not in range [" << min << ';' << max << "]\n";
        n = get_num<T>(prompt);
    }
    return n;
}

////////////////////////////////////////////////////////////////////////////////

static inline void keep_window_open()
{
    std::cout << "Press <RETURN> to close this window...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

////////////////////////////////////////////////////////////////////////////////

}  // namespace util
}  // namespace mps

////////////////////////////////////////////////////////////////////////////////

#endif // MPS_UTIL_STREAMUTILS_HPP

////////////////////////////////////////////////////////////////////////////////
