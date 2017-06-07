#include "Calculator.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "mps/str_util.hpp"
#include "mps/clipboard.hpp"
#include "mps/console_util.hpp"

#include "math_util.hpp"
#include "types.hpp"

using std::cin;
using std::cout;

Calculator::Calculator()
    : parser{ symbolTable }
{
    register_commands();

    if (std::ifstream prompt_stream{ "prompt.txt" })  // ommitted error handling, since there is a default prompt in place
        getline(prompt_stream, prompt);

    if (std::ifstream intro_stream{ "intro.txt" }) {
        std::ostringstream s;
        s << intro_stream.rdbuf();
        intro = s.str();
    }

    parser.on_result([this](const auto& n) {
        parser.symbol_table().set_var("_", n);
        parser.symbol_table().set_var("ans", n);
        print_complex(cout, n);
        cout << '\n';
    });
}

void Calculator::run(int argc, char* argv[])
{
    switch (argc) {
    case 1:
        run_cli();
        break;
    case 2:
        if (std::string(argv[1]) == "-")
            run_cli();
        else if (!run_file(argv[1]))
            parser.parse(argv[1]);
        break;
    default:
        throw std::runtime_error{ "Invalid number of arguments" };
    }
}

bool Calculator::run_file(const std::string& path)
{
    if (std::ifstream ifs{ path }) {
        parser.parse(ifs);
        return true;
    }
    return false;
}

void Calculator::run_cli()
{
    cout << intro 
         << "Copyright (C) 2017 Matthias Stauber\n"
            "This program comes with ABSOLUTELY NO WARRANTY\n"
         << prompt;

    for (std::string s; std::getline(cin, s); ) {
        s = mps::str::trim(s);
        try {
            if (s.size() && !handle_cmd(s))
                parser.parse(s);
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << '\n';
        }
        cout << prompt;
    }
}

bool Calculator::handle_cmd(const std::string& cmd)
{
    auto found = commands.find(mps::str::tolower(cmd));
    if (found != end(commands)) {
        found->second();
        return true;
    }
    return false;
}

void Calculator::register_commands()
{
    static const std::string helpText{
        "For a list of operators, commands and functions please view the readme file\n"
    };

    commands["help"] = [] { std::cout << helpText; };

    commands["clear"] = commands["cls"] = [this] {
        mps::cls();
        cout << intro;
    };

    commands["clear all"] = [this] {
        parser.symbol_table().clear();
        mps::cls(); 
        cout << intro;
    };
    commands["clear vars"] = [this] { parser.symbol_table().clear_vars(); };
    commands["clear funcs"] = [this] { parser.symbol_table().clear_funcs(); };
    commands["clear lists"] = [this] { parser.symbol_table().clear_lists(); };

    commands["hide vars"] = [this] { parser.set_vardef_is_res(false); };
    commands["show vars"] = [this] { parser.set_vardef_is_res(true); };

    commands["ls"] = [this] {
        const auto& vars = parser.symbol_table().vars();
        if (vars.size())
            cout << "Variables:\n~~~~~~~~~~\n";
        for (const auto& v : vars) {
            cout << "  " << v.first << " = ";
            print_complex(cout, v.second.value);
            cout << '\n';
        }
        
        const auto& funcs = parser.symbol_table().funcs();
        if (funcs.size())
            cout << "\nFunctions:\n~~~~~~~~~~\n";
        for (const auto& f : funcs)
            cout << "  " << f.second << '\n';

        const auto& lists = parser.symbol_table().lists();
        if (lists.size())
            cout << "\nLists:\n~~~~~~\n";
        for (const auto& l : lists) {
            cout << "  " << l.first << " = ";
            print_list(cout, l.second);
            cout << '\n';
        }
    };

    commands["run"] = [this] {
        std::string fname;
        if (cout << "file: " && std::getline(cin, fname))
            run_file(fname);
    };

    commands["copy"] = [this] {
        auto&& str = mps::str::to_string(parser.symbol_table().value_of("ans"));
        mps::set_clipboard_text(std::move(str));
    };

    commands["copy,"] = [this] {
        auto&& str = mps::str::to_string(parser.symbol_table().value_of("ans"));
        mps::set_clipboard_text(mps::str::format_number_EU(std::move(str)));
    };

    commands["table"] = [this] {
        // const auto func = mps::get_str("Function: ");
        // double low = mps::get_num<double>("Low: ");
        // double high = mps::get_num<double>("High: ");
        cout << "Sorry, table feature not implemented yet.\n";
    };

    commands["dec"] = [] {
        cout << "hex/bin (W/ leading 0): ";
        int val{};
        if (mps::read_hex(cin, val))
            cin.ignore();
        else if (!mps::read_bin(cin, val)) {
            mps::recover_line(cin);
            return;
        }
        std::cout << val << '\n';
    };

    commands["bin"] = [] {
        cout << "dec/hex: ";
        int val{};
        if (!mps::read_hex(cin, val) && !(cin >> val)) {
            mps::recover_line(cin);
            return;
        }
        cin.ignore();
        mps::print_binary(cout, val);
        cout << '\n';
    };

    commands["hex"] = [] {
        cout << "dec/bin (w/ leading 0): ";
        int val{};
        if (!mps::read_bin(cin, val)) {
            if (!(cin >> val)) {
                mps::recover_line(cin);
                return;
            }
            cin.ignore();
        }
        cout << "0x" << std::hex << val << std::dec << '\n';
    };

    commands["exp"] = [this] {
        if (!parser.has_result())
            return;
        cout << abs(parser.result()) 
             << "*e^(" 
             << deg(arg(parser.result())) 
             << "deg)i\n";
    };
}
