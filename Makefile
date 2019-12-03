ROOTDIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BUILDDIR=$(ROOTDIR)/build
OBJS_DIR_RELEASE=$(BUILDDIR)/.objs
OBJS_DIR_DEBUG=$(BUILDDIR)/.dobjs

GTEST_INCLUDE_PATH=/usr/include
GTEST_LIB_PATH=/usr/lib

#### compilation flags

CFLAGS = -Wall -std=c++17 -flto -I/usr/local/include
CFLAGS_RELEASE = $(CFLAGS) -Ofast -DNDEBUG
CFLAGS_DEBUG = $(CFLAGS) -g

LFLAGS = -pthread -flto -L/usr/local/lib -ltensorflow
LFLAGS_RELEASE = $(LFLAGS) -Ofast
LFLAGS_DEBUG = $(LFLAGS) -g

ENGINE_RELEASE=$(BUILDDIR)/chessplusplus
ENGINE_DEBUG=$(BUILDDIR)/chessplusplus_debug

TEST_RELEASE=$(BUILDDIR)/test
TEST_DEBUG=$(BUILDDIR)/test_debug

ENGINE_SOURCES=$(wildcard src/*.cpp)
ENGINE_OBJS_RELEASE=$(ENGINE_SOURCES:src/%.cpp=$(OBJS_DIR_RELEASE)/src/%.o)
ENGINE_OBJS_DEBUG=$(ENGINE_SOURCES:src/%.cpp=$(OBJS_DIR_DEBUG)/src/%.o)

TEST_SOURCES=$(wildcard tests/*.cpp)
TEST_OBJS_RELEASE=$(TEST_SOURCES:tests/%.cpp=$(OBJS_DIR_RELEASE)/tests/%.o)
TEST_OBJS_DEBUG=$(TEST_SOURCES:tests/%.cpp=$(OBJS_DIR_DEBUG)/tests/%.o)

all: release debug

release: engine test
debug: engine_debug test_debug

engine: $(ENGINE_RELEASE)
engine_debug: $(ENGINE_DEBUG)

test: $(TEST_RELEASE)
test_debug: $(TEST_DEBUG)

#### engine

$(ENGINE_RELEASE): $(ENGINE_OBJS_RELEASE) | $(OBJS_DIR_RELEASE)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS_RELEASE)

$(ENGINE_DEBUG): $(ENGINE_OBJS_DEBUG) | $(OBJS_DIR_DEBUG)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS_DEBUG)

#### test

$(TEST_RELEASE): $(TEST_OBJS_RELEASE) $(filter-out $(OBJS_DIR_RELEASE)/src/engine.o,$(ENGINE_OBJS_RELEASE)) | $(OBJS_DIR_RELEASE)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS_RELEASE) -L$(GTEST_LIB_PATH) -lgtest

$(TEST_DEBUG): $(TEST_OBJS_DEBUG) $(filter-out $(OBJS_DIR_DEBUG)/src/engine.o,$(ENGINE_OBJS_DEBUG)) | $(OBJS_DIR_DEBUG)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS_DEBUG) -L$(GTEST_LIB_PATH) -lgtest

#### objs

$(OBJS_DIR_RELEASE)/src/%.o : src/%.cpp | $(OBJS_DIR_RELEASE)/src
	@echo "Compiling release $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_RELEASE)

$(OBJS_DIR_DEBUG)/src/%.o : src/%.cpp | $(OBJS_DIR_DEBUG)/src
	@echo "Compiling debug $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_DEBUG)

$(OBJS_DIR_RELEASE)/tests/%.o : tests/%.cpp | $(OBJS_DIR_RELEASE)/tests
	@echo "Compiling release $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_RELEASE) -Isrc

$(OBJS_DIR_DEBUG)/tests/%.o : tests/%.cpp | $(OBJS_DIR_DEBUG)/tests
	@echo "Compiling debug $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_DEBUG) -Isrc

#### dirs

$(OBJS_DIR_RELEASE):
	mkdir -p $@

$(OBJS_DIR_DEBUG):
	mkdir -p $@

$(OBJS_DIR_RELEASE)/src:
	mkdir -p $@

$(OBJS_DIR_DEBUG)/src:
	mkdir -p $@

$(OBJS_DIR_RELEASE)/tests:
	mkdir -p $@

$(OBJS_DIR_DEBUG)/tests:
	mkdir -p $@


#### utils

clean:
	$(RM) -r $(BUILDDIR)

ctags:
	ctags -R --tag-relative=yes --exclude=.git $(ROOTDIR)

tidy:
	clang-tidy-8 $(ENGINE_SOURCES) -header-filter=src/*.h -checks='*' -warnings-as-errors='*'

help:
	@echo "Available commands:"
	@echo "\trelease - compile engine and tests"
	@echo "\tdebug   - compile engine and tests in debug mode"
	@echo "\tall     - release + debug"
	@echo "\tclean   - remove all files created by make"
	@echo "\tctags   - create tags for all files"


.PHONY: release engine_release tests_release
.PHONY: debug engine_debug tests_debug
.PHONY: clean ctags help
