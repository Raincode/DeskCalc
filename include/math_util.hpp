#pragma once

#include <cmath>
#include <numeric>

#include "types.hpp"

constexpr bool is_zero(const Complex& num);

Complex safe_div(Complex left, Complex right);
Complex safe_floordiv(Complex left, Complex right);
Complex safe_mod(Complex left, Complex right);

template<class T>
T safe_mod(T left, T right)
{
    if (!right)
        throw std::runtime_error{ "Divide by Zero" };
    return left % right;
}

Complex sqr(Complex num);

double factorial(int n);
Complex factorial(Complex num);
Complex impedance_parallel(Complex R1, Complex R2);

std::size_t len(const ComplexList& list);
Complex sum(const ComplexList& list);
Complex avg(const ComplexList& list);
Complex standard_deviation(const ComplexList& list);

constexpr double pi{ 3.1415926535897932385 };

constexpr double deg(double rad)
{
    return 180 / pi * rad;
}

constexpr double rad(double deg)
{
    return pi / 180 * deg;
}

namespace temp {
    constexpr double absoluteZero{ -273.1499999999999773 };

    constexpr double CtoK(double celsius) { return celsius - absoluteZero; }
    constexpr double KtoC(double kelvin) { return kelvin + absoluteZero; }
    constexpr double CtoF(double celsius) { return 1.8 * celsius + 32; }
    constexpr double FtoC(double fahrenheit) { return (fahrenheit - 32) / 1.8; }
    constexpr double FtoK(double fahrenheit) { return CtoK(FtoC(fahrenheit)); }
    constexpr double KtoF(double kelvin) { return CtoF(KtoC(kelvin)); }
}
