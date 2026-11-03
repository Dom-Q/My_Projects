cmake_minimum_required(VERSION 3.24)
project(trading_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Strict warnings for our own code only (not for GoogleTest).
# Use it by linking it to a target:
#   target_link_libraries(<your target> PRIVATE strict_warnings)
add_library(strict_warnings INTERFACE)
if(MSVC)
  target_compile_options(strict_warnings INTERFACE /W4 /permissive-)
else()
  target_compile_options(strict_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
endif()

# GoogleTest, the C++ testing library. Downloaded on the first configure.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# Your libraries and tests go below. To register a test program with ctest:
#   target_link_libraries(<your test target> PRIVATE GTest::gtest_main)
#   gtest_discover_tests(<your test target>)
