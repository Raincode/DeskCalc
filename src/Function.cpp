#include "Function.hpp"

#include "Parser.hpp"
#include "SymbolTable.hpp"

Complex Function::operator()(const Args& args) const
{
    if (args.size() != vars.size())
        throw std::runtime_error{ "Function '" + funcName + "' - Expected " + std::to_string(vars.size()) +
        " input arguments (received " + std::to_string(args.size()) + ")" };
    Parser parser{ table };
    VarGuard guard{ table };
    for (std::size_t i = 0; i < vars.size(); ++i)
        guard.set_temp(vars[i], args[i]);
    parser.parse(term);
    return parser.result();
}