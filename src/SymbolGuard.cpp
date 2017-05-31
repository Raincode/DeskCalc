#include "SymbolGuard.hpp"

SymbolGuard::SymbolGuard(SymbolTable& table)
    : table{ table }
{
}

SymbolGuard::~SymbolGuard()
{
    for (const auto& var : shadowedVars)
        table.remove_var(var);

    for (const auto& pair : varCache) {
        if (pair.second.isConst)
            table.add_const(pair.first, pair.second.val);
         else
            table.set_var(pair.first, pair.second.val);
    }
}

void SymbolGuard::shadow_var(const std::string& name, Complex tempVal)
{
    if (table.has_var(name)) {
        auto val = table.value_of(name);
        varCache[name] = table.is_const(name) ? make_const(val) : make_var(val);
        table.remove_var(name);
    }
    table.set_var(name, tempVal);
    shadowedVars.push_back(name);
}