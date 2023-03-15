cmake_minimum_required(VERSION 3.15..3.21)

project(Kilo 
    VERSION 0.1 
    DESCRIPTION "A simple modal text editor" 
    LANGUAGES CXX
)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 17)

    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_subdirectory(src)

# add_subdirectory(app)
