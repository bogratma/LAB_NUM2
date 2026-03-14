cmake_minimum_required(VERSION 4.0)
project(LAB_NUM2)

set(CMAKE_CXX_STANDARD 20)

add_executable(LAB_NUM2 main.cpp
        tree/SyntaxTree.cpp
        tree/SyntaxTree.h
        tree/Node.cpp
        tree/Node.h)
