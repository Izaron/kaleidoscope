add_library(codegen codegen.cc)

target_include_directories(codegen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS ast)
target_link_libraries(codegen PUBLIC ${LIBS})

enable_testing()

add_executable(
    codegen_test
    codegen_ut.cc
)

llvm_map_components_to_libnames(llvm_libs core)

target_link_libraries(
    codegen_test
    gtest_main
    codegen
    lexer
    parser
    ${llvm_libs}
)

include(GoogleTest)
gtest_discover_tests(codegen_test)
