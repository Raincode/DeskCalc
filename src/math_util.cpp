#include "math_util.hpp"

#include <stdexcept>

using std::runtime_error;

constexpr bool is_zero(const Complex& num)
{
    return !num.real() && !num.imag();
}

Complex safe_div(Complex left, Complex right)
{
    if (is_zero(right))
        throw runtime_error{ "Divide by zero" };
    return left / right;
}

Complex safe_floordiv(Complex left, Complex right)
{
    if (is_zero(right))
        throw runtime_error{ "Divide by zero" };
    left /= right;
    return { std::floor(left.real()), std::floor(left.imag()) };
}

Complex safe_mod(Complex left, Complex right)
{
    if (left.imag() || right.imag()) 
        throw runtime_error{ "Modulo not defined for complex numbers (yet)" };

    auto ltrunc = std::trunc(left.real());
    auto rtrunc = std::trunc(right.real());

    if (left.real() != ltrunc || right.real() != rtrunc)
        throw runtime_error{ "Modulo not defined for floating point numbers" };

    return safe_mod(static_cast<long>(ltrunc), static_cast<long>(rtrunc));
}

double factorial(int n)
{
    if (n < 0) throw runtime_error{ "Factorial not defined for negative numbers" };
    double ret{ 1 };
    for (int i = 2; i <= n; ++i)
        ret *= i;
    return ret;
}

Complex factorial(Complex num)
{
    auto trunc = std::trunc(num.real());
    if (num.imag() || trunc < 0 || trunc != num.real())
        throw runtime_error{ "Factorial only defined for natural numbers (and zero)" };
    return factorial(static_cast<int>(trunc));
}

Complex impedance_parallel(Complex R1, Complex R2)
{
    if (is_zero(R1) && is_zero(R2))
        throw runtime_error{ "Resistors must be greater than 0" };
    return R1 * R2 / (R1 + R2);
}

Complex sqr(Complex num)
{
    return num * num;
}

std::size_t len(const ComplexList& list)
{
    return list.size();
}

Complex sum(const ComplexList& list)
{
    return std::accumulate(cbegin(list), cend(list), Complex{});
}

Complex avg(const ComplexList& list)
{
    return sum(list) / static_cast<double>(len(list));
}

Complex standard_deviation(const ComplexList& list)
{
    const auto acc_distance_squared = [a = avg(list)](auto sum, auto next) 
    { return sum + sqr(next - a); };

    const auto s_sqr = std::accumulate(cbegin(list), cend(list), Complex{}, acc_distance_squared)
                       / static_cast<double>(len(list) - 1);

    return std::sqrt(s_sqr);
}