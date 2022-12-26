cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(chessplusplus VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug" "RelWithDebInfo")
set(LOG_LEVEL 0 CACHE STRING "Logging level")

add_compile_options(-Wall -Wextra -pedantic -Werror -flto -march=native -mtune=native)
add_compile_options("-DLOG_LEVEL=${LOG_LEVEL}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-Ofast -DDEBUG -g)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Ofast -DNDEBUG)
else()
    message("Unknown build type : ${CMAKE_BUILD_TYPE}")
endif()

configure_file(chessplusplusConfig.h.in chessplusplusConfig.h)

file(GLOB engine_src "engine/*.cpp")
list(REMOVE_ITEM engine_src "${PROJECT_SOURCE_DIR}/engine/main.cpp")

add_library(engine_objs OBJECT ${engine_src})
target_include_directories(engine_objs
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/engine")

# engine
add_executable(chessplusplus engine/main.cpp)
target_link_libraries(chessplusplus PUBLIC engine_objs)
target_include_directories(chessplusplus
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/engine")

# regression
file(GLOB regression_src "tools/regression/*.cpp")
add_executable(regression ${regression_src})
target_link_libraries(regression PUBLIC engine_objs)
target_include_directories(regression
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/tools/regression"
        "${PROJECT_SOURCE_DIR}/engine")

enable_testing()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

# tests
file(GLOB tests_src "tests/*.cpp")
add_executable(unitTests ${tests_src})
target_link_libraries(unitTests
    PUBLIC
        engine_objs
    PRIVATE
        GTest::GTest)
target_include_directories(unitTests
    PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/tests"
        "${PROJECT_SOURCE_DIR}/engine")

configure_file(tests/run_perft_tests.sh run_perft_tests.sh)
configure_file(tests/run_search_tests.sh run_search_tests.sh)

add_test(NAME unitTests COMMAND "./unitTests")
add_test(NAME perftTests COMMAND "./tests/run_perft_tests.sh" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME searchTests COMMAND "./tests/run_search_tests.sh" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
