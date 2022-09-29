add_executable(kilo)

target_sources(kilo
    PUBLIC 
        main.cpp Terminal.cpp 
        Editor.cpp
    PRIVATE 
        "${Kilo_SOURCE_DIR}/includes/Terminal.hpp" "${Kilo_SOURCE_DIR}/includes/Editor.hpp"
)

target_include_directories(kilo PUBLIC ../includes)

target_link_libraries(kilo PRIVATE libposix)

target_compile_features(kilo PRIVATE cxx_std_20)

target_compile_options(kilo PRIVATE -Wall -Wextra)