cmake_minimum_required(VERSION 3.0)
project(babai)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native")
include_directories(include)
# test classes
add_executable(test test/problem-test.cpp)
add_executable(example1 examples/example1.cpp)
target_link_libraries(test gsl gslcblas gmp gmpxx)
target_link_libraries(example1 gsl gslcblas gmp gmpxx)
