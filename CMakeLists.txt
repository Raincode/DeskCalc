cmake_minimum_required(VERSION 3.0)

include_directories(include)

set(CALC_SRC
    src/main.cpp
    src/Calculator.cpp
)

set(MATH_PARSER_SRC
    src/Parser.cpp
    src/TokenStream.cpp
    src/SymbolTable.cpp
    src/Function.cpp
    src/SymbolGuard.cpp
    src/math_util.cpp
)

set(TEST_SRC
    test/main.cpp
    test/math_util_Test.cpp
    test/Parser_Test.cpp
    test/TokenStream_Test.cpp
    test/SymbolTable_Test.cpp
    test/SymbolGuard_Test.cpp
)

project(DeskCalc)

add_library(MathParser ${MATH_PARSER_SRC})

add_executable(${PROJECT_NAME} ${CALC_SRC})
add_executable("Tests" ${TEST_SRC})

target_link_libraries(${PROJECT_NAME} MathParser)
target_link_libraries("Tests" MathParser)
