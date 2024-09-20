cmake_minimum_required(VERSION 3.5)

project(logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/headers)
include_directories(${PROJECT_SOURCE_DIR}/sources)

set(HEADERS
    headers/log.h
    headers/log_printer.h
    headers/console_printer.h
    headers/scope_timer.h
    headers/file_printer.h
)

set(SOURCES
    sources/log.cpp
    sources/console_printer.cpp
    sources/scope_timer.cpp
    sources/file_printer.cpp
)

add_executable(multithread_logger main.cpp ${SOURCES} ${HEADERS})
