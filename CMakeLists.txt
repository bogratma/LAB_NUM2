cmake_minimum_required(VERSION 3.20)
project(LAB_NUM2)
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)
set(CMAKE_CXX_STANDARD 20)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
endif()
set(COMMON_SOURCES
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
        MDFA/MDFA.h
        Regex/Regex.cpp
        Regex/Regex.h
        tree/ParserResult.h
        StateElimination/StateElim.cpp
        StateElimination/StateElim.h
        MDFA/Product.h
)
add_executable(LAB_NUM2 main.cpp ${COMMON_SOURCES})
add_executable(unit_tests tests/tests.cpp ${COMMON_SOURCES})
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)