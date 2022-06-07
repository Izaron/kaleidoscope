add_library(codegen codegen.cc)

target_include_directories(codegen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS ast)
target_link_libraries(codegen PUBLIC ${LIBS})
