ROOTDIR=$(shell dirname $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
OUTDIR=$(ROOTDIR)/build

GTEST_LIB_PATH=/usr/lib

NUM_THREADS=8

#### outputs

LIB_STATIC_RELEASE = $(OUTDIR)/libengine_static.a
LIB_STATIC_DEBUG   = $(OUTDIR)/libengine_static_debug.a


#### compilation flags

CFLAGS_COMMON = -Wall -std=c++17 -flto -I$(ROOTDIR)/engine
LFLAGS_COMMON = -pthread -flto

CFLAGS_RELEASE = $(CFLAGS_COMMON) -Ofast -DNDEBUG -DNUM_THREADS=$(NUM_THREADS)
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -DNUM_THREADS=$(NUM_THREADS)

OUTDIR_PROGRAMS=$(OUTDIR)/programs_objs
OUTDIR_ENGINE=$(OUTDIR)/engine_objs
OUTDIR_TESTS=$(OUTDIR)/tests_objs
