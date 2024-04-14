cmake_minimum_required(VERSION 3.26)
project(SCM)

set(CMAKE_CXX_STANDARD 17)

add_executable(SCM main.cpp
        Model.cpp
        Model.hpp)
