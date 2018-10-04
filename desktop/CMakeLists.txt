cmake_minimum_required(VERSION 3.12)
project(desktop)

set(CMAKE_CXX_STANDARD 14)

add_executable(desktop source/main.cpp source/ArduinoHandler.cpp headers/ArduinoHandler.h)