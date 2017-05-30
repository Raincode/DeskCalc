#pragma once

#include <complex>
#include <functional>
#include <vector>

using Complex = std::complex<double>;
using ComplexFunc = std::function<Complex(const Complex&)>;
using ComplexList = std::vector<Complex>;
using Args = std::vector<Complex>;
using ComplexMultiFunc = std::function<Complex(const Args&)>;
using ComplexListFunc = std::function<Complex(const ComplexList& list)>;