cmake_minimum_required(VERSION 3.5.0)
project(notnan VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(notnan main.cpp)

include(CTest)
enable_testing()

find_package(Catch2 REQUIRED)
add_executable(notnan_test tests/notnan_test.cpp)
target_link_libraries(notnan_test PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(notnan_test)

add_test(
    NAME dummy
    COMMAND true
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
