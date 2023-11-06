ROOT_DIR = $(shell dirname $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
BUILD_DIR = $(ROOT_DIR)/build

CC ?= gcc
CXX ?= g++

CFLAGS = -Wall -std=c++20 -flto -Ofast -DNDEBUG -DLOG_LEVEL=0
LFLAGS = -pthread -flto -Ofast

BINARY = $(BUILD_DIR)/chessplusplus.exe

SOURCES = $(wildcard *.cpp)
OBJS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o) | $(BUILD_DIR)

all: $(BINARY)

$(BINARY): $(OBJS)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS)

$(BUILD_DIR)/%.o : %.cpp | $(BUILD_DIR)
	@echo "Compiling $<"
	@$(CXX) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR):
	mkdir -p $@