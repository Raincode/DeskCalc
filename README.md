# DeskCalc
Desk(top) Calculator inspired by "The C++ Programming Language" (Stroustrup)

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
```

## Features
* Variables
* Functions (multiple parameters possible)
* Complex Number arithmetic
* Minimal list support

## Built-in Constants
pi, e, i, deg (will convert to rad, so you can write sin(90deg))

## Built-in Functions

__Trigonometric:__ sin, cos, tan, asin, acos, atan, sinh, cosh, tanh, asinh, acosh, atanh

__Temperature Conversion:__ CtoF, CtoK, FtoC, FtoK, KtoC, KtoF

__Lists:__ avg, len, sum, sum2 (squared sum), sx (standard deviation), ux (standard uncertainty)

__Complex:__ Re, Im, arg, abs, norm

__Misc.:__ ln, log, sqr, sqrt, cbrt, round, ceil, floor, trunc

## Commands
* __copy__
* __copy,__
* __bin__
* __dec__
* __hex__
* __clear/cls__
