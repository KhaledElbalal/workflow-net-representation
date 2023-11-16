cmake_minimum_required(VERSION 3.26)
project(PetriNets)

set(CMAKE_CXX_STANDARD 20)

add_executable(PetriNets src/main.cpp
        src/Petrinet.cpp
        include/Petrinet.h
        include/Constants.h
        include/Types.h
        src/Utility.cpp
        include/Utility.h)
