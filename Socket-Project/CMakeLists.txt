﻿# CMakeList.txt : CMake project for Socket-Project, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Client "Client.cpp" "Client.h" "Parser.h"  "socket-helper.h")
add_executable (Server "Server.cpp" "Server.h" "Parser.h"  "socket-helper.h")

# TODO: Add tests and install targets if needed.
set(CMAKE_CXX_FLAGS "-pthread")