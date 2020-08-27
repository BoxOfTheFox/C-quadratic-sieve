cmake_minimum_required(VERSION 3.16)
project(quadratic-sieve C)

set(CMAKE_C_STANDARD 11)

add_library(lgmp STATIC main.c list.h list.c)
add_library(lm STATIC main.c)
target_link_libraries(lgmp lm)
add_executable(quadratic-sieve main.c list.h list.c)
target_link_libraries(quadratic-sieve gmp m)