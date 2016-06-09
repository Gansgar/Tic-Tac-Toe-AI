cmake_minimum_required(VERSION 3.5)
project(tic_tac_toe)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(SOURCE_FILES main.cpp TicTacToe.cpp TicTacToe.h SearchTree.h)
add_executable(tic_tac_toe ${SOURCE_FILES})

target_link_libraries(tic_tac_toe ${CURSES_LIBRARIES})