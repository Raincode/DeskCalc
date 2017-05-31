#pragma once

#include <functional>
#include <map>
#include <string>

#include "Parser.hpp"

class Calculator {
public: 
    Calculator();
    void run(int argc, char* argv[]);

private:
    void show_intro() const;
    bool handle_cmd(const std::string& cmd);
    void register_commands();

    bool run_file(const std::string& path);
    void run_cli();
    void exec(const std::string& input);

    SymbolTable symbolTable;
    Parser parser;
    std::map<std::string, std::function<void()>> commands;
};