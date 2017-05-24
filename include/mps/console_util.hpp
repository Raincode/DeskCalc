////////////////////////////////////////////////////////////////////////////////
// Provides convenient classes and functions for writing console apps
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

#ifndef MPS_CONSOLE_UTIL_HPP
#define MPS_CONSOLE_UTIL_HPP

////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <initializer_list>
#include <functional>
#include <map>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <mps/str_util.hpp>
#include <mps/stream_util.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace {
////////////////////////////////////////////////////////////////////////////////

void cls()
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
    
template<typename MenuOptID>
class ConsoleMenu {
public:
    using MenuHandler = std::function<bool(MenuOptID)>;
    using CmdHandler = std::function<bool(const std::string&)>;
    using MenuOption = std::pair<MenuOptID, std::string>;

    ConsoleMenu() = default;
    ConsoleMenu(const std::initializer_list<MenuOption>& options);

    void add_opt(const MenuOption &opt) { menu[opt.first] = opt.second; }

    void set_menu_handler(const MenuHandler &f) { menu_handler = f; }
    void set_cmd_handler(const CmdHandler &f) { cmd_handler = f; }

    void run();
    void quit() { is_running = false; }

    void set_info(const std::string &i) { info = i; }

    void print();

private:
    std::string compose_menu() const;
    void handle_input(const std::string &input);

    ////////////////////////////////////////////////////////////////////////////
    
    bool is_running{true};
    std::string msg;    // information displayed one time only and then cleared
    std::string info;   // persistant information displayed above menu
    std::map<MenuOptID, std::string> menu;
    MenuHandler menu_handler;
    CmdHandler cmd_handler;
};

////////////////////////////////////////////////////////////////////////////////

template<typename T>
ConsoleMenu<T>::ConsoleMenu(const std::initializer_list<MenuOption> &options)
{
    for (const auto &opt : options)
        add_opt(opt);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConsoleMenu<T>::print()
{
    mps::cls();
    std::cout << info << compose_menu() << "> ";
    msg.clear();
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConsoleMenu<T>::handle_input(const std::string &input)
{
    int choice{};
    if (!menu_handler || !mps::parse_int(input, choice) || !menu_handler(T(choice)))
        if (!cmd_handler || !cmd_handler(input))
            msg = "[!] Unknown option/command";
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConsoleMenu<T>::run()
{
    std::string line;
    while (is_running)
    {
        print();
        if (!std::getline(std::cin, line)) break;
        handle_input(line);
    }
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::string ConsoleMenu<T>::compose_menu() const
{
    std::string result{"    MENU\n"};
    for (const auto &opt : menu)
        result += "[" + std::to_string(int(opt.first)) + "] " + opt.second + "\n";
    return result;
}

////////////////////////////////////////////////////////////////////////////////
}   /* anonymous namespace */
}   /* namespace mps */
////////////////////////////////////////////////////////////////////////////////

#endif // MPS_CONSOLE_UTIL_HPP

////////////////////////////////////////////////////////////////////////////////
