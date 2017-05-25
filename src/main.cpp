#include <iostream>
#include <stdexcept>

#include "Calculator.hpp"

int main(int argc, char* argv[])
try
{
    Calculator calc;
    calc.run_cli();
    return 0;
}
catch (std::runtime_error& e)
{
    std::cerr << e.what() << std::endl;
    return -1;
}
catch (...)
{
    std::cerr << "Unknown exception!" << std::endl;
    return -2;
}