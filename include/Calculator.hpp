#pragma once

#include <functional>
#include <map>
#include <string>

#include "Parser.hpp"
#include "SymbolTable.hpp"

class Calculator {
public: 
    Calculator();
    void run(int argc, char* argv[]);

private:
    bool handle_cmd(const std::string& cmd);
    void register_commands();

    bool run_file(const std::string& path);
    void run_cli();

    SymbolTable symbolTable;
    Parser parser;
    std::string prompt{ "> " };
    std::string intro;
    std::map<std::string, std::function<void()>> commands;
};