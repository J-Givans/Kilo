add_executable(kilo)

find_package(fmt REQUIRED)

target_sources(kilo
    PUBLIC 
        Terminal.cpp Utils.cpp Offset.cpp
        Editor.cpp main.cpp
    PRIVATE 
        "${Kilo_SOURCE_DIR}/includes/Terminal.hpp" 
        "${Kilo_SOURCE_DIR}/includes/Utils.hpp"
        "${Kilo_SOURCE_DIR}/includes/Keys.hpp"
        "${Kilo_SOURCE_DIR}/includes/Editor.hpp"
        "${Kilo_SOURCE_DIR}/includes/Vector2.hpp"
        "${Kilo_SOURCE_DIR}/includes/Offset.hpp"
)

target_include_directories(kilo PUBLIC ../includes)

target_link_libraries(kilo 
    PRIVATE libposix
    PRIVATE fmt::fmt
)

target_compile_features(kilo PRIVATE cxx_std_20)

target_compile_options(kilo PRIVATE -Wall -Wextra -Wconversion)