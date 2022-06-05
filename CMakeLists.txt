cmake_minimum_required(VERSION 3.0)

project(parser_ppse LANGUAGES C)

add_executable(parser_ppse main.c)

install(TARGETS parser_ppse RUNTIME DESTINATION bin)