add_library(ast ast.cc)

target_include_directories(ast INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS source)
target_link_libraries(ast PUBLIC ${LIBS})
