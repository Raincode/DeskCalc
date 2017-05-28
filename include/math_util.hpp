#pragma once

#include <complex>

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

unsigned factorial(int n);
Complex factorial(Complex num);
Complex impedance_parallel(Complex R1, Complex R2);