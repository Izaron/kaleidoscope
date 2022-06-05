cmake_minimum_required(VERSION 3.14)

project(kaleidoscope)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

# list subdirectories
add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(noncopyable)
add_subdirectory(parser)
add_subdirectory(source)

# enable gtest (for testing)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/c9461a9b55ba954df0489bab6420eb297bed846b.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
