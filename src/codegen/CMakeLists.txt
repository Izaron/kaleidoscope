add_library(codegen codegen.cc)

target_include_directories(codegen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

#llvm_map_components_to_libnames(llvm_libs core ipo)
llvm_map_components_to_libnames(llvm_libs ${LLVM_TARGETS_TO_BUILD} core ipo support x86asmparser x86codegen x86desc x86disassembler x86info)

list(APPEND LIBS ast)
target_link_libraries(codegen PUBLIC ${LIBS} ${llvm_libs})

enable_testing()

add_executable(
    codegen_test
    codegen_ut.cc
)

target_link_libraries(
    codegen_test
    gtest_main
    codegen
    lexer
    parser
)

include(GoogleTest)
gtest_discover_tests(codegen_test)
