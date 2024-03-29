# DeskCalc
Desk(top) Calculator inspired by "The C++ Programming Language" (Stroustrup)

## Example Build Guide with CMake
Assumes you have a suitable compiler toolchain and cmake installed.
```
git clone ...
mkdir DeskCalc/build
cd DeskCalc/build
cmake ..
make -j or ninja -jX
./Tests
./DeskCalc
```

## Quick Start Guide
```
// calculate arbitrary expressions
>> 1 + 2(3 + 4) + 5pi - pi*e / (1 + 27*2 + 3) - sin(45deg)
29.8536

// define and use variables
>> var = 42
42
>> 2*var + 2var
168

// complex number arithmetic
>> 5i*(3-i) + (1+i) + i^3
6+15i 
>> 5*e^(90deg)i + 3*e^(45deg)i - 1*e^(135deg)i
2.82843+6.41421i
>> exp
7.01015*e^(66.2043deg)i

// define functions with multiple variables
>> fn f(a,b,c) = a*b + c
>> f(2,4,3)
11

// define lists and do basic calculations on them
>> x = [1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5]
>> x
[1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5]
>> x = [for i=1, 5:0.5 i]
>> x
[1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5]
>> len(x); avg(x); sum(x)
9
3
27
>> sx(x); ux(x)
1.36931
0,456435

// using negative values in list ranges is possible, but requires use of parentheses (-x) to be parsable as primary
>> x = [for i=(-15),(-30):(-1) i]
>> x
[-15, -16, -17, -18, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, -30]
```

## Features
* Variables
* Functions (multiple parameters possible)
* Complex Number arithmetic
* Minimal list support

## Built-in Operators
* Add `+`
* Sub `-`
* Mul `*`
* Div `/`
* Pow `^` or `**`
* Floordiv `//` or `div`
* Mod `%` or `mod`
* Fac `!` (unary, suffix)
* Neg `-` (unary, prefix)
* Parallel impedance `||`, where A || B == A*B/(A+B)

## Built-in Constants
pi, e, i, deg (will convert to rad, so you can write sin(90deg))

## Built-in Functions

__Trigonometric:__ sin, cos, tan, asin, acos, atan, sinh, cosh, tanh, asinh, acosh, atanh

__Temperature Conversion:__ CtoF, CtoK, FtoC, FtoK, KtoC, KtoF

__Lists:__ avg, len, sum, sum2 (squared sum), sx (standard deviation), ux (standard uncertainty)

__Complex:__ Re, Im, arg, abs, norm

__Misc.:__ ln, log, sqr, sqrt, cbrt, round, ceil, floor, trunc, sgn

## Commands
* __copy:__ Copy the last result to clipboard using '.' as decimal point
* __copy,:__ Copy the last result to clipboard using ',' as decimal point
* __bin:__ Output a hexadecimal or decimal number as binary
* __dec:__ Output a hexadecimal or binary number as decimal
* __hex:__ Output a decimal or binary number as hexadecimal
* __clear/cls:__ Clears the screen from previous results
* __clear (all | vars | funcs | lists):__ Removes all user-defined variables/functions/lists
* __run:__ Run a DeskCalc file while running the CLI
* __ls:__ List variables, user-defined functions and lists
* __exp:__ Output last result in expontential Form r*e^(tetha in °)i
