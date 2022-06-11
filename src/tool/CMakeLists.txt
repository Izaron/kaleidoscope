add_executable(tool tool.cc)

#llvm_map_components_to_libnames(llvm_libs support x86info x86codegen x86asmparser)

list(APPEND LIBS codegen lexer parser)
#target_link_libraries(tool PUBLIC ${LIBS} ${llvm_libs})
target_link_libraries(tool PUBLIC ${LIBS})
