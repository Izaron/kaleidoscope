add_library(parser parser.cc)

target_include_directories(parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS ast lexer)
target_link_libraries(parser PUBLIC ${LIBS})

enable_testing()

add_executable(
    parser_test
    parser_ut.cc
)

target_link_libraries(
    parser_test
    gtest_main
    parser
    dump
)

include(GoogleTest)
gtest_discover_tests(parser_test)
