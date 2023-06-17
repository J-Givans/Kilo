# Make new library called lib
add_library(lib)

# Source and header files for lib
target_sources(lib 
    PRIVATE 
        lib/read/read.hpp
        lib/read/read.cpp
        lib/write/write.hpp
        lib/write/write.cpp
        lib/ioctl/ioctl.hpp
        lib/ioctl/ioctl.cpp
        lib/tcgetattr/tcgetattr.hpp
        lib/tcgetattr/tcgetattr.cpp
        lib/tcsetattr/tcsetattr.hpp
        lib/tcsetattr/tcsetattr.cpp
        lib/winsize/winsize.hpp
        lib/winsize/winsize.cpp
)

# Needed to compile lib
target_include_directories(lib PUBLIC lib)

# C++ standard to use
target_compile_features(lib PRIVATE cxx_std_17)

# Extra compiler flags; warnings
target_compile_options(lib PRIVATE -Wall -Wextra)

######################################################

find_package(fmt REQUIRED)

find_package(Microsoft.GSL REQUIRED)

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
    PRIVATE
        lib
        fmt::fmt
        Microsoft.GSL::GSL
)

target_compile_features(kilo PRIVATE cxx_std_20)

target_compile_options(kilo PRIVATE -Wall -Wextra -Wconversion)