add_executable(tests)

find_package(GTest REQUIRED 
    COMPONENTS 
        gtest_main gmock_main
)

find_package(fmt REQUIRED)

target_link_libraries(tests
    PUBLIC
        GTest::gtest_main
        GTest::gmock_main
    PRIVATE
        fmt::fmt
)

target_include_directories(tests
    PUBLIC
        "${PROJECT_SOURCE_DIR}/includes"
)

target_sources(tests
    PUBLIC
        Terminal.test.cpp

    PRIVATE
        "${PROJECT_SOURCE_DIR}/includes/Terminal/Terminal.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.cpp"
)

target_compile_features(tests PRIVATE cxx_std_20)

target_compile_options(tests PRIVATE -Wall -Werror -Wextra)