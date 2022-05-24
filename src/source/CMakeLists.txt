add_library(source source.cc)

target_include_directories(source INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND LIBS noncopyable)
target_link_libraries(source PUBLIC ${LIBS})

enable_testing()

add_executable(
    source_test
    source_ut.cc
)

target_link_libraries(
    source_test
    gtest_main
    source
)

include(GoogleTest)
gtest_discover_tests(source_test)
