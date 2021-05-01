# main paths
ROOTDIR=$(shell dirname $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
BUILD_DIR=$(ROOTDIR)/build
OBJS_DIR_RELEASE=$(BUILD_DIR)/.objs
OBJS_DIR_DEBUG=$(BUILD_DIR)/.dobjs

# google test
GTEST_INCLUDE_PATH ?= /usr/local/include
GTEST_LIB_PATH     ?= /usr/local/lib

# complier
CC  ?= gcc
CXX ?= g++

# compliation flags
CFLAGS = -Wall -std=c++17 -flto
CFLAGS_RELEASE = $(CFLAGS) -Ofast -DNDEBUG
CFLAGS_DEBUG = $(CFLAGS) -g -DDEBUG

LFLAGS = -pthread -flto
LFLAGS_RELEASE = $(LFLAGS) -Ofast
LFLAGS_DEBUG = $(LFLAGS) -g

LCOV_CFLAGS = -fprofile-arcs -ftest-coverage
LCOV_LFLAGS = --coverage


