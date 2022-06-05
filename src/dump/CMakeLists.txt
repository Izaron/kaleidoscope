add_library(dump dump.cc)

target_include_directories(dump INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS ast)
target_link_libraries(dump PUBLIC ${LIBS})

enable_testing()

add_executable(
    dump_test
    dump_ut.cc
)

target_link_libraries(
    dump_test
    gtest_main
    dump
)

include(GoogleTest)
gtest_discover_tests(dump_test)
