TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
TARGET = dcalc

QMAKE_CXXFLAGS += -Wextra -Wno-unused-function

SOURCES += main.cpp \
    TokenStream.cpp \
    Parser.cpp \
    Calculator.cpp

HEADERS += \
    TokenStream.hpp \
    Token.hpp \
    Parser.hpp \
    Calculator.hpp \
    strutil.hpp \
    clipboard.hpp \
    Function.hpp

DISTFILES += \
    README
