# Make new library called libposix
add_library(libposix)

# Source and header files for libposix
target_sources(libposix 
    PRIVATE lib/lib.cpp 
    PUBLIC lib/lib.cpp
)

# Needed to compile libposix
target_include_directories(libposix PUBLIC lib)

# C++ standard to use
target_compile_features(libposix PRIVATE cxx_std_17)

# Extra compiler flags; warnings
target_compile_options(libposix PRIVATE -Wall -Wextra)

######################################################

add_executable(kilo)

find_package(fmt REQUIRED)

target_sources(kilo
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Terminal/Terminal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Offset/Offset.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Editor/Editor.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/includes/Terminal/Terminal.hpp
        ${PROJECT_SOURCE_DIR}/includes/Utils/Utils.hpp
        ${PROJECT_SOURCE_DIR}/includes/Keys/Keys.hpp
        ${PROJECT_SOURCE_DIR}/includes/Editor/Editor.hpp
        ${PROJECT_SOURCE_DIR}/includes/Vec2/Vec2.hpp
        ${PROJECT_SOURCE_DIR}/includes/Offset/Offset.hpp
)

target_include_directories(kilo PUBLIC ../includes)

target_link_libraries(kilo 
    PRIVATE libposix
    PRIVATE fmt::fmt
)

target_compile_features(kilo PRIVATE cxx_std_20)

target_compile_options(kilo PRIVATE -Wall -Wextra -Wconversion)