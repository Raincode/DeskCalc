#include <iostream>
#include <stdexcept>

int main()
try
{

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