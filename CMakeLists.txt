cmake_minimum_required(VERSION 3.14)

project(uzuki
    VERSION 1.0.0
    DESCRIPTION "C++ library for R list JSON format validation"
    LANGUAGES CXX)

add_subdirectory(extern)

add_library(uzuki INTERFACE)

target_include_directories(uzuki INTERFACE include/)

target_link_libraries(uzuki INTERFACE nlohmann_json::nlohmann_json)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()
