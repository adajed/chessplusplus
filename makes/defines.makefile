ROOTDIR=$(shell dirname $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
OUTDIR=$(ROOTDIR)/build

GTEST_LIB_PATH=/usr/lib


#### outputs

LIB_STATIC_RELEASE = $(OUTDIR)/libengine_static.a
LIB_STATIC_DEBUG   = $(OUTDIR)/libengine_static_debug.a


#### compilation flags
CC=g++

CFLAGS_COMMON = -Wall -std=c++17 -I$(ROOTDIR)/includes
LFLAGS_COMMON = -pthread

CFLAGS_RELEASE = $(CFLAGS_COMMON) -Ofast
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -DDEBUG

OUTDIR_PROGRAMS=$(OUTDIR)/programs_objs
OUTDIR_ENGINE=$(OUTDIR)/engine_objs
OUTDIR_TESTS=$(OUTDIR)/tests_objs
