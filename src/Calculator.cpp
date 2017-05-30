#include "Calculator.hpp"

#include <fstream>
#include <iostream>

#include "mps/str_util.hpp"
#include "mps/clipboard.hpp"
#include "mps/console_util.hpp"

#include "math_util.hpp"
#include "Warning.hpp"

using namespace std;

Calculator::Calculator()
    : parser{ symbolTable }
{
    register_commands();
    symbolTable.add_constants();
    symbolTable.add_default_funcs();
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
            exec(argv[1]);
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
    static const string prompt{ "> " };
    show_intro();
    cout << prompt;

    for (string s; getline(cin, s); ) {
        s = mps::str::trim(s);
        if (s.size() && !handle_cmd(s))
            exec(s);
        cout << prompt;
    }
}

void Calculator::exec(const std::string& input)
{
    try {
        parser.parse(input);
        if (parser.has_result()) {
            parser.symbol_table().set_var("_", parser.result());
            parser.symbol_table().set_var("ans", parser.result());
            print_complex(cout, parser.result());
        }
    }
    catch (runtime_error& e) {
        cerr << e.what() << '\n';
    }
}

void Calculator::show_intro() const
{
    static const string intro{ "\n                                Desk Calc"
                               "\n                               ~ ~ ~ ~ ~ ~\n" };
    ifstream ifs{ "intro.txt" };
    if (ifs) {
        for (string s; getline(ifs, s); )
            cout << "                " << s << '\n';
        cout << '\n';
    }
    else
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

static const string helpText{
    "For a list of operators, commands and functions view readme.txt\n"
};

void Calculator::register_commands()
{
    commands["clear"] = commands["cls"] = [this] {
        mps::cls();
        show_intro();
    };

    commands["help"] = [] { std::cout << helpText; };

    commands["run"] = [this] {
        cout << "file: ";
        string fname;
        if (cin >> fname) {
            cin.ignore(1);	// cin leaves '\n' in buffer
            run_file(fname);
        }
    };

    commands["copy"] = [this] {
        auto str = mps::str::to_string(parser.symbol_table().value("ans"));
        mps::set_clipboard_text(str);
    };

    commands["copy,"] = [this] {
        auto str = mps::str::to_string(parser.symbol_table().value("ans"));
        mps::set_clipboard_text(mps::str::format_number_EU(str));
    };

    commands["table"] = [this] {
        string func;
        cout << "Function: ";
        if (!(cin >> func))
            return;


    };
}

void print_complex(std::ostream& os, Complex n)
{
    using std::cout;

    if (n.imag()) {
        if (n.real()) {
            cout << n.real();
            if (n.imag() > 0)
                cout << '+';
        }
        if (std::abs(n.imag()) != 1)
            cout << n.imag();
        cout << 'i';
    }
    else
        cout << n.real();
    cout << '\n';
}