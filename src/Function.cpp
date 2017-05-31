#include "Function.hpp"

#include "Parser.hpp"
#include "SymbolGuard.hpp"
#include "SymbolTable.hpp"

Function::Function(const std::string& name, SymbolTable& table)
    : funcName{ name }, table{ table }
{
}

Complex Function::operator()(const List& args) const
{
    if (args.size() != vars.size())
        throw std::runtime_error{ funcName + " expects " + std::to_string(vars.size()) +
        " arguments (received " + std::to_string(args.size()) + ")" };
    Parser parser{ table };
    SymbolGuard guard{ table };
    for (std::size_t i = 0; i < vars.size(); ++i)
        guard.shadow_var(vars[i], args[i]);
    parser.parse(term);
    return parser.result();
}