cmake_minimum_required(VERSION 3.16)

project(GuessANumber LANGUAGES C)

add_executable(GuessANumber main.c utility.h utility.c settings.h)

install(TARGETS GuessANumber
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
