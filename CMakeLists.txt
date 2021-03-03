cmake_minimum_required(VERSION 3.15)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lncurses")
project(BoDealer)
add_subdirectory(walrus)
