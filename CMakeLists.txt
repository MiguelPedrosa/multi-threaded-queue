cmake_minimum_required(VERSION 3.22)
project(multi-threaded-queue)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
string(APPEND CMAKE_CXX_FLAGS "-O2 -Wall")


include_directories(include)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test_pairs
  tests/single.cpp
)
target_link_libraries(
  test_pairs
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_pairs)