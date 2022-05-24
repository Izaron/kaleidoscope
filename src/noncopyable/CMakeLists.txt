add_library(noncopyable noncopyable.cc)

target_include_directories(noncopyable INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
