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
    symbolTable.set_var("deg", pi / 180);

    std::ifstream ifs{ "prompt.txt" };
    std::getline(ifs, prompt);
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
    show_intro();
    cout << prompt;

    for (string s; getline(cin, s); ) {
        s = mps::str::trim(s);
        try {
            if (s.size() && !handle_cmd(s))
                exec(s);
        }
        catch (runtime_error& e) {
            cerr << e.what() << '\n';
        }
        cout << prompt;
    }
}

void Calculator::exec(const std::string& input)
{
    parser.parse(input);
    if (parser.has_result()) {
        parser.symbol_table().set_var("_", parser.result());
        parser.symbol_table().set_var("ans", parser.result());
        print_complex(cout, parser.result());
        cout << '\n';
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

void print_binary(int n)
{
    if (n <= 1) {
        cout << n;
        return;
    }
    print_binary(n >> 1);
    cout << n % 2;
}

bool read_hex(std::istream& is, int& out)
{
    char c1, c2;
    if (!is.get(c1))
        return false;
    if (!is.get(c2)) {
        is.putback(c1);
        return false;
    }

    if (c1 == '0' && c2 == 'x')
        return (is >> hex >> out >> dec).good();

    is.putback(c2);
    is.putback(c1);
    return false;
}

void Calculator::register_commands()
{
    commands["clear"] = commands["cls"] = [this] {
        mps::cls();
        show_intro();
    };

    commands["help"] = [] { std::cout << helpText; };

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

    commands["plot"] = [this] {
        std::cout << "coming soon...\n";
    };

    commands["dec"] = [] {

    };

    commands["bin"] = [] {
        cout << "dec/hex: ";
        int val;
        if (!read_hex(cin, val) && !(cin >> val)) {
            mps::recover_line(cin);
            return;
        }
        cin.ignore();
        print_binary(val);
        cout << '\n';
    };

    commands["hex"] = [] {
        static Warning w{ "Note: Binary numbers must start with '0'" };
        cout << "  >> ";
        char c;
        int val = 0;
        if (std::cin >> c && c == '0') {
            if (std::cin >> c && c == 'x') {
                if (!(std::cin >> std::hex >> val >> std::dec))
                    return;
            }
            else {
                std::string bin;
                if (!std::getline(std::cin, bin))
                    return;
                for (std::size_t i = 0; i < bin.size(); ++i)
                    if (bin[i] != '0')
                        val += static_cast<int>(std::pow(2, bin.size() - i - 1));
            }
        }
        std::cout << std::hex << val << std::dec << '\n';
    };
}