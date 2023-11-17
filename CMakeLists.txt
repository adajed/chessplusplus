cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(chessplusplus VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug" "RelWithDebInfo")
set(LOG_LEVEL 0 CACHE STRING "Logging level")
set(ECO_CODES_FILE "${PROJECT_SOURCE_DIR}/tools/regression/scid.eco" CACHE STRING "File with ECO codes")
set(ARCH_NATIVE True CACHE STRING "Build for native arch")
set_property(CACHE ARCH_NATIVE PROPERTY STRINGS "False" "True")
set(ARCH_SSE False CACHE STRING "Build for SSE")
set_property(CACHE ARCH_SSE PROPERTY STRINGS "False" "True")
set(ARCH_MMX False CACHE STRING "Build for MMX")
set_property(CACHE ARCH_MMX PROPERTY STRINGS "False" "True")
set(ARCH_SSE2 False CACHE STRING "Build for SSE2")
set_property(CACHE ARCH_SSE2 PROPERTY STRINGS "False" "True")
set(ARCH_SSSE3 False CACHE STRING "Build for SSSE3")
set_property(CACHE ARCH_SSSE3 PROPERTY STRINGS "False" "True")
set(ARCH_SSE41 False CACHE STRING "Build for SSE41")
set_property(CACHE ARCH_SSE41 PROPERTY STRINGS "False" "True")
set(ARCH_AVX2 False CACHE STRING "Build for AVX2")
set_property(CACHE ARCH_AVX2 PROPERTY STRINGS "False" "True")
set(ARCH_AVXVNNI False CACHE STRING "Build for AVXVNNI")
set_property(CACHE ARCH_AVXVNNI PROPERTY STRINGS "False" "True")
set(ARCH_AVX512 False CACHE STRING "Build for AVX512")
set_property(CACHE ARCH_AVX512 PROPERTY STRINGS "False" "True")
set(ARCH_VNNI256 False CACHE STRING "Build for VNNI256")
set_property(CACHE ARCH_VNNI256 PROPERTY STRINGS "False" "True")
set(ARCH_VNNI512 False CACHE STRING "Build for VNNI512")
set_property(CACHE ARCH_VNNI512 PROPERTY STRINGS "False" "True")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(BUILD_FOR_WINDOWS True)
endif()

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -flto -march=native -mtune=native)
endif()
add_compile_definitions("LOG_LEVEL=${LOG_LEVEL}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        add_compile_options(/Od /Ob1)
    else()
        add_compile_options(-g)
    endif()
    add_compile_definitions(DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    if (MSVC)
        add_compile_options(/O2 /Ot /Ob2)
    else()
        add_compile_options(-Ofast -g)
    endif()
    add_compile_definitions(DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        add_compile_options(/O2 /Ot /Ob2)
    else()
        add_compile_options(-Ofast)
    endif()
    add_compile_definitions(NDEBUG)
else()
    message("Unknown build type : ${CMAKE_BUILD_TYPE}")
endif()

file(GLOB engine_src "engine/*.cpp")
list(REMOVE_ITEM engine_src "${PROJECT_SOURCE_DIR}/engine/main.cpp")

add_library(engine_objs OBJECT ${engine_src})
target_include_directories(engine_objs
    PUBLIC "${PROJECT_SOURCE_DIR}/engine")

# engine
add_executable(chessplusplus engine/main.cpp)
target_link_libraries(chessplusplus PUBLIC engine_objs)
target_include_directories(chessplusplus
    PUBLIC "${PROJECT_SOURCE_DIR}/engine")

# regression
if (NOT BUILD_FOR_WINDOWS)
    file(GLOB regression_src "tools/regression/*.cpp")
    add_executable(regression ${regression_src})
    target_compile_options(regression PRIVATE "-DECO_CODES_FILE=\"${ECO_CODES_FILE}\"")
    target_link_libraries(regression PUBLIC engine_objs)
    target_include_directories(regression
        PUBLIC
            "${PROJECT_SOURCE_DIR}/tools/regression"
            "${PROJECT_SOURCE_DIR}/engine")
endif()

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
