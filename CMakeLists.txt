cmake_minimum_required(VERSION 4.0)
project(LAB_NUM2)

set(CMAKE_CXX_STANDARD 20)

add_executable(LAB_NUM2 main.cpp
        tree/Node.cpp
        tree/Node.h
        NFA/State.cpp
        NFA/State.h
        NFA/NFA.cpp
        NFA/NFA.h
        NFA/Chunk.h
        DFA/DFA.cpp
        DFA/DFA.h
        MDFA/MDFA.cpp
        MDFA/MDFA.h)
