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

#include "Calculator.hpp"

#include <algorithm>
#include <fstream>

#include "clipboard.hpp"
#include "strutil.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace mps {

void cls()  // utility function clear screen // TODO: make cross-platform
{   // unix: std::cout << "\x1B[2J\x1B[H";  ?
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO buf_info{};
    if (!GetConsoleScreenBufferInfo(hConsole, &buf_info)) return;

    auto conSize = static_cast<DWORD>(buf_info.dwSize.X * buf_info.dwSize.Y);
    DWORD charsWritten{};
    COORD coordScreen{0, 0};
    if (!FillConsoleOutputCharacter(hConsole, ' ', conSize, coordScreen, &charsWritten)) return;

    if (!GetConsoleScreenBufferInfo(hConsole, &buf_info)) return;
    if (!FillConsoleOutputAttribute(hConsole, buf_info.wAttributes, conSize, coordScreen, &charsWritten)) return;
    SetConsoleCursorPosition(hConsole, coordScreen);
}

namespace math {

////////////////////////////////////////////////////////////////////////////////

static std::string intro{
        "\n                                Desk Calculator"
        "\n                               ~ ~ ~ ~ ~ ~ ~ ~ ~\n"
};

static std::string helptext{
     "                    Operators\n"
     "                    ~~~~~~~~~\n"
     "+  -  *  /  ()\n! (Factorial)\n|| (parallel resistances)\n\n"
     "                    Commands\n"
     "                    ~~~~~~~~\n"
     "COPY\t\tCopy last result to clipboard\n"
     "COPY,\t\tSpecial format copy (european); e.g. 1.2e+020 -> 1,2E20\n"
     "CLEAR|CLS\tClears the console from previous operations\n"
     "CLEARVAR\tDelete all user-defined variables\n"
     "CLEARALL\tClear screen and remove all user defined variables\n"
     "LS\t\tList all currently defined variables\n\n"
     "fn\t\tDefine functions; usage: fn name(var) = term\n\n"
};

void Calculator::show_intro() const
{
//    std::ifstream greeting{"intro.txt"};
//    if (!greeting)
//    {
        std::cout << intro;
//    }
//    else
//    {
//        for (std::string line; std::getline(greeting, line) ;)
//            std::cout << "                " << line << '\n';
//        std::cout << '\n';
//    }
}

////////////////////////////////////////////////////////////////////////////////

bool Calculator::process_command(const std::string& command)
{
    auto cmd = util::tolower(command);
    if (cmd.size() > 3 && cmd.substr(cmd.size() - 4, 4) == ".txt")
    {
        std::ifstream ifs{cmd};
        if (ifs)
        {
            run(ifs);
        }
        else std::cerr << "Unable to open file '" << cmd << "'\n";
        return true;
    }
    else if (cmd == "restart")
    {
        cls();
        mParser.clear_vars();
        show_intro();
        return true;
    }
    else if (cmd == "clear" || cmd == "cls")
    {
        cls();
        std::cout << intro;
        return true;
    }
    else if (cmd == "clearvar")
    {
        mParser.clear_vars();
        return true;
    }
    else if (cmd == "clearall")
    {
        cls();
        std::cout << intro;
        mParser.clear_vars();
        return true;
    }
    else if (cmd == "ls")
    {
        auto vars = mParser.get_vars();
        for (const auto& v : vars)
        {
            if (v.first != "_" && v.first != "Ans") std::cout << v.first << " = " << v.second << '\n';
        }
        return true;
    }
    else if (cmd == "copy")
    {
        set_clipboard_text(util::to_string(mParser.get_var("_")));
        return true;
    }
    else if (cmd == "copy,")
    {
        auto str = util::to_string(mParser.get_var("_"));
        set_clipboard_text(util::european_format(str));
        return true;
    }
    else if (cmd == "help")
    {
        std::cout << helptext;
        return true;

    }
    else if (cmd == "quit" || cmd == "exit" || cmd == "close" || cmd == "end")
    {
        mIsRunning = false;
        return true;
    }
    else if (mParser.isfunc(cmd))
    {   // Call function on last result when entered without argument
        if (mParser.isvar("_"))
        {
            std::cout << mParser.get_func(cmd)(mParser.get_var("_")) << '\n';
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

void Calculator::run_cli()
{
    static const std::string prompt {"> "};
    std::cout << prompt;
    for (std::string line; mIsRunning && std::getline(std::cin, line) ;)
    {
        try
        {
            exec(line);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }

        if (mIsRunning) std::cout << prompt;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Calculator::exec(const std::string& arg)
{
    if (arg.size() && !process_command(arg))
    {
        mParser.token_stream().set_input(arg);
        while (mParser.parse_stmt())
        {
            if (mParser.hasResult())
            {
                mParser.set_var("_", mParser.result());
                mParser.set_var("Ans", mParser.result());
                std::cout << mParser.result() << '\n';
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Calculator::run(std::istream& input, std::ostream& os)
{
    mTokenStream.set_input(input);
    while (true)
    {
        try
        {
            if (mParser.parse_stmt())
            {
                if (mParser.hasResult())
                {
                    os << mParser.result() << '\n';
                }
            }
            else break;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << "\n> ";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace math
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
