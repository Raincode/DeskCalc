#pragma once

#include <complex>
#include <functional>
#include <iostream>
#include <vector>

using Complex = std::complex<double>;
using List = std::vector<Complex>;
using Func = std::function<Complex(const List&)>;

template<class T>
void print_complex(std::ostream& os, const std::complex<T>& n)
{
    using std::cout;

    if (n.imag()) {
        if (n.real()) {
            cout << n.real();
            if (n.imag() > 0)
                cout << '+';
        }
        if (std::abs(n.imag()) != 1)
            cout << n.imag();
        cout << (n.imag() == -1 ? "-" : "") << 'i';
    }
    else
        cout << n.real();
}

template<class T>
void print_list(std::ostream& os, const std::vector<T>& v)
{
    std::cout << '[';
    std::string sep;
    for (const auto& item : v) {
        std::cout << sep;
        print_complex(std::cout, item);
        sep = ", ";
    }
    std::cout << ']';
}