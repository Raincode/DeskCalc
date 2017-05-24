#ifndef MPS_CONSOLE_MENU_HPP
#define MPS_CONSOLE_MENU_HPP

#include <algorithm>
#include <initializer_list>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include <mps/StringUtils.hpp>
#include <mps/StreamUtils.hpp>

namespace mps {

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

private:
    bool is_running{true};
    std::string msg;    // information displayed one time only and then cleared
    std::string info;   // persistant information displayed above menu
    std::map<MenuOptID, std::string> menu;
    MenuHandler menu_handler;
    CmdHandler cmd_handler;
};

template<typename T>
ConsoleMenu<T>::ConsoleMenu(const std::initializer_list<MenuOption> &options)
{
    for (const auto &opt : options)
        add_opt(opt);
}

template<typename T>
void ConsoleMenu<T>::print()
{
    mps::util::cls();
    std::cout << info << compose_menu() << "> ";
    msg.clear();
}

template<typename T>
void ConsoleMenu<T>::handle_input(const std::string &input)
{
    int choice{};
    if (!menu_handler || !mps::util::parse_num<int>(input, choice) || !menu_handler(T(choice)))
        if (!cmd_handler || !cmd_handler(input))
            msg = "[!] Unknown option/command";
}

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

template<typename T>
std::string ConsoleMenu<T>::compose_menu() const
{
    std::string result{"    MENU\n"};
    for (const auto &opt : menu)
        result += "[" + std::to_string(int(opt.first)) + "] " + opt.second + "\n";
    return result;
}

} // namespace mps

#endif // MPS_CONSOLE_MENU_HPP
