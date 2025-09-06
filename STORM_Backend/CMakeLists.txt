cmake_minimum_required(VERSION 3.14)
project(storm_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------- Build server --------
add_executable(server
    src/server.cpp
    src/store.cpp
)
target_include_directories(server PRIVATE src)

# -------- GoogleTest --------
find_package(GTest REQUIRED)
enable_testing()

add_executable(test_store
    tests/test_store.cpp
    src/store.cpp
)
target_include_directories(test_store PRIVATE src)
target_link_libraries(test_store GTest::GTest GTest::Main pthread)

include(GoogleTest)
gtest_discover_tests(test_store)
