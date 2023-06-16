add_executable(tests)

find_package(GTest REQUIRED 
    COMPONENTS 
        gtest_main gmock_main
)

target_link_libraries(tests
    PUBLIC
        GTest::gtest_main
        GTest::gmock_main
)

target_include_directories(tests
    PUBLIC
        "${PROJECT_SOURCE_DIR}/includes"
)

target_sources(tests
    PUBLIC
        Terminal.test.cpp
)

target_compile_features(tests PRIVATE cxx_std_20)

target_compile_options(tests PRIVATE -Wall -Werror -Wextra)