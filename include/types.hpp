#pragma once

#include <complex>
#include <functional>
#include <vector>

using Complex = std::complex<double>;
using ComplexFunc = std::function<Complex(const Complex&)>;
using Args = std::vector<Complex>;
using ComplexMultiFunc = std::function<Complex(const Args&)>;