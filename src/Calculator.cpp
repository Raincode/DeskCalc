#include "Calculator.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "mps/str_util.hpp"
#include "mps/clipboard.hpp"
#include "mps/console_util.hpp"

#include "math_util.hpp"
#include "types.hpp"
#include "Warning.hpp"

using namespace std;

Calculator::Calculator()
    : parser{ symbolTable }
{
    register_commands();

    ifstream ifs{ "prompt.txt" };  // ommitted error handling, since there is a default prompt in place
    getline(ifs, prompt);

    if (ifstream ifs{ "intro.txt" }) {
        std::ostringstream s;
        s << ifs.rdbuf();
        intro = s.str();
    }

    parser.on_result([this](auto&& n) {
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
        if (string(argv[1]) == "-")
            run_cli();
        else if (!run_file(argv[1]))
            parser.parse(argv[1]);
        break;
    default:
        throw runtime_error{ "Invalid number of arguments" };
    }
}

bool Calculator::run_file(const string& path)
{
    ifstream ifs{ path };
    if (!ifs)
        return false;
    parser.parse(ifs);
    return true;
}

void Calculator::run_cli()
{
    show_intro();
    cout << prompt;

    for (string s; getline(cin, s); ) {
        s = mps::str::trim(s);
        try {
            if (s.size() && !handle_cmd(s))
                parser.parse(s);
        }
        catch (runtime_error& e) {
            cerr << e.what() << '\n';
        }
        cout << prompt;
    }
}

void Calculator::show_intro() const
{
    cout << intro;
    static Warning w{ "Copyright (C) 2017 Matthias Stauber\n"
                      "This program comes with ABSOLUTELY NO WARRANTY\n" };
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
    static const string helpText{
        "For a list of operators, commands and functions please view the readme file\n"
    };

    commands["help"] = [] { std::cout << helpText; };

    commands["clear"] = commands["cls"] = [this] {
        mps::cls();
        show_intro();
    };

    commands["clear all"] = [this] {
        parser.symbol_table().clear();
        mps::cls(); 
        show_intro();
    };
    commands["clear vars"] = [this] { parser.symbol_table().clear_vars(); };
    commands["clear funcs"] = [this] { parser.symbol_table().clear_funcs(); };
    commands["clear lists"] = [this] { parser.symbol_table().clear_lists(); };

    commands["hide vars"] = [this] { parser.set_vardef_is_res(false); };
    commands["show vars"] = [this] { parser.set_vardef_is_res(true); };

    commands["ls"] = [this] {
        auto&& vars = parser.symbol_table().vars();
        if (vars.size())
            cout << "Variables:\n~~~~~~~~~~\n";
        for (const auto& v : vars) {
            cout << "  " << v.first << " = ";
            print_complex(cout, v.second);
            cout << '\n';
        }
        
        auto&& funcs = parser.symbol_table().funcs();
        if (funcs.size())
            cout << "\nFunctions:\n~~~~~~~~~~\n";
        for (const auto& f : funcs)
            cout << "  " << f.second << '\n';

        auto&& lists = parser.symbol_table().lists();
        if (lists.size())
            cout << "\nLists:\n~~~~~~\n";
        for (const auto& l : lists) {
            cout << "  " << l.first << " = ";
            print_list(cout, l.second);
            cout << '\n';
        }
    };

    commands["run"] = [this] {
        string fname;
        if (cout << "file: " && getline(cin, fname))
            run_file(fname);
    };

    commands["copy"] = [this] {
        auto str = mps::str::to_string(parser.symbol_table().value_of("ans"));
        mps::set_clipboard_text(str);
    };

    commands["copy,"] = [this] {
        auto str = mps::str::to_string(parser.symbol_table().value_of("ans"));
        mps::set_clipboard_text(mps::str::format_number_EU(str));
    };

    commands["table"] = [this] {
        string func = mps::get_str("Function: ");
        double low = mps::get_num<double>("Low: ");
        double high = mps::get_num<double>("High: ");
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
        cout << "0x" << hex << val << dec << '\n';
    };

    commands["exp"] = [this] {
        if (!parser.has_result())
            return;
        auto && n = parser.result();
        cout << abs(n) << "*e^(" << deg(arg(n)) << "deg)i\n";
    };
}