#include "Calculator.hpp"

#include <fstream>
#include <iostream>

#include "mps/str_util.hpp"
#include "mps/console_util.hpp"

using namespace std;

Calculator::Calculator()
{
	register_commands();
}

void Calculator::run_file(const string& path)
{

}

void Calculator::run_cli()
{
	static const string prompt{ "> " };
	show_intro();
	cout << prompt;

	for (string s; isRunning && getline(cin, s) ;) {
		if (s.size() && !handle_cmd(s)) {
			try {
				parser.parse(s);
				print_complex(cout, parser.result());
			}
			catch (runtime_error& e) {
				std::cerr << e.what() << '\n';
			}
		}
		if (isRunning) cout << prompt;
	}
}

struct Warning {
	Warning(const string& msg) { std::cout << msg; }
};

void Calculator::show_intro() const
{
	static const string intro{ "\n                                Desk Calc"
							   "\n                               ~ ~ ~ ~ ~ ~\n" };

	ifstream ifs{ "intro.txt" };
	if (ifs) {
		for (string s; getline(ifs, s) ;)
			std::cout << "                " << s << '\n';
		std::cout << '\n';
	}
	else cout << intro;

	static Warning w{ "Copyright (C) 2017  Matthias Stauber\n"
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
	commands["rip"] = commands["quit"] = commands["exit"] = commands["close"] =
		[this] { isRunning = false; };
	commands["license"] = 
		[] { std::cout << "GNU General Public License v3\n"; };
	commands["copyright"] = 
		[] { std::cout << "Copyright (C) 2017  Matthias Stauber\n"; };
	commands["clear"] = commands["cls"] = 
		[this] { 
			mps::cls();
			show_intro();
		};
	commands["help"] =
		[] { std::cout << helpText; };
}

void print_complex(std::ostream& os, Complex num)
{
	using std::cout;
	auto re = num.real();
	auto im = num.imag();

	if (im) {
		if (re) cout << re;
		if (re && im > 0) cout << '+';
		if (im != 1) std::cout << im;
		std::cout << 'i';
	}
	else {
		std::cout << re;
	}
	std::cout << '\n';
}