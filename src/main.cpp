#include <iostream>
#include <stdexcept>

#include "Calculator.hpp"

int main(int argc, char* argv[])
try {
    Calculator calc;
    calc.run(argc, argv);
}
catch (const std::runtime_error& e) {
    std::cerr << e.what() << '\n';
    return -1;
}
catch (...) {
    std::cerr << "Unknown exception!\n";
    return -2;
}
