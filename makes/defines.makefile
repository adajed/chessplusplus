# main paths
ROOTDIR=$(shell dirname $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
BUILD_DIR=$(ROOTDIR)/build
OBJS_DIR_RELEASE=$(BUILD_DIR)/.objs
OBJS_DIR_DEBUG=$(BUILD_DIR)/.dobjs

# google test
GTEST_INCLUDE_PATH ?= /usr/local/include
GTEST_LIB_PATH     ?= /usr/local/lib

# complier
COMP ?= g++

# compliation flags
CFLAGS = -Wall -std=c++17 -flto
CFLAGS_RELEASE = $(CFLAGS) -Ofast -DNDEBUG
CFLAGS_DEBUG = $(CFLAGS) -g -DDEBUG

LFLAGS = -pthread -flto -Wall -std=c++17
LFLAGS_RELEASE = $(LFLAGS) -Ofast -DNDEBUG
LFLAGS_DEBUG = $(LFLAGS) -g -DDEBUG

LCOV_CFLAGS = -fprofile-arcs -ftest-coverage
LCOV_LFLAGS = --coverage

# clang format
CLANG_FORMAT ?= clang-format
CLANG_FORMAT_ARGS = -style=file -i

# clang tidy
CLANG_TIDY ?= clang-tidy
CLANG_TIDY_ARGS = -std=c++17

USE_AVX2 ?= no
