# Make new library called libposix
add_library(libposix)

# Source and header files for libposix
target_sources(libposix 
    PRIVATE lib.cpp 
    PUBLIC "${Kilo_SOURCE_DIR}/includes/posix"
)

# Needed to compile libposix
target_include_directories(libposix PUBLIC ../includes)

# C++ standard to use
target_compile_features(libposix PRIVATE cxx_std_17)

# Extra compiler flags; warnings
target_compile_options(libposix PRIVATE -Wall -Wextra)