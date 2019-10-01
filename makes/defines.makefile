ROOTDIR=$(shell dirname $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
OUTDIR=$(ROOTDIR)/build

GTEST_LIB_PATH=/usr/lib


#### compilation flags
CC=g++

CFLAGS_COMMON = -Wall -std=c++17
LFLAGS_COMMON =

CFLAGS_RELEASE = $(CFLAGS_COMMON) -O3
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g

OUTDIR_ENGINE=$(OUTDIR)/engine_objs
OUTDIR_TESTS=$(OUTDIR)/tests_objs
