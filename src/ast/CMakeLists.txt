add_library(ast ast.cc ast_dump.cc)

target_include_directories(ast INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS source)
target_link_libraries(ast PUBLIC ${LIBS})

enable_testing()

add_executable(
    ast_test
    ast_ut.cc
)

target_link_libraries(
    ast_test
    gtest_main
    ast
)

include(GoogleTest)
gtest_discover_tests(ast_test)
