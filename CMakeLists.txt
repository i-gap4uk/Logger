cmake_minimum_required(VERSION 3.5)

project(logger VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/headers)
include_directories(${PROJECT_SOURCE_DIR}/sources)

set(HEADERS
    headers/core/log.h
    headers/interfaces/log_printer.h
    headers/output/console_printer.h
    headers/utils/scope_timer.h
    headers/output/file_printer.h
)

set(SOURCES
    sources/core/log.cpp
    sources/output/console_printer.cpp
    sources/utils/scope_timer.cpp
    sources/output/file_printer.cpp
)

# Option to build the static library
option(BUILD_STATIC_LIB "Build the static library" ON)

# Option to build the examples (main.cpp)
option(BUILD_EXAMPLES "Build the logger's usage examples" OFF)

if(BUILD_STATIC_LIB)
    add_library(logger STATIC ${SOURCES})
    target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/headers)
    install(TARGETS logger
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/headers/ DESTINATION headers)
endif()

if(BUILD_EXAMPLES)
    add_executable(logger_examples main.cpp ${SOURCES} ${HEADERS})
endif()
