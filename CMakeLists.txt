cmake_minimum_required(VERSION 3.15)

project(Kilo VERSION 0.1 LANGUAGES CXX)

add_compile_options(-std=c++17 -Wall -Wextra)
add_executable(kilo main.cpp Terminal.cpp Terminal.hpp lib.hpp lib.cpp)