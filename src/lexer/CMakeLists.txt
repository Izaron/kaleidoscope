add_library(lexer lexer.cc)

target_include_directories(lexer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS source)
target_link_libraries(lexer PUBLIC ${LIBS})

enable_testing()

add_executable(
    lexer_test
    lexer_ut.cc
)

target_link_libraries(
    lexer_test
    gtest_main
    lexer
)

include(GoogleTest)
gtest_discover_tests(lexer_test)
