include ./makes/defines.makefile

all: release debug

release: lib engine test tools
debug: lib_debug engine_debug test_debug tools_debug

lib:
	@+make -C engine lib_release

lib_debug:
	@+make -C engine lib_debug
#### engine

engine: lib
	@+make -C engine engine_release

engine_debug: lib_debug
	@+make -C engine engine_debug


#### test

test: lib
	@+make -C tests release

test_debug: lib_debug
	@+make -C tests debug


#### tools

tools: lib
	@+make -C tools release

tools_debug: lib_debug
	@+make -C tools debug

#### utils

clean:
	$(RM) -r $(BUILD_DIR)

ctags:
	ctags -R --tag-relative=yes --exclude=.git $(ROOTDIR)

tidy:
	clang-tidy-8 $(ENGINE_SOURCES) -header-filter=src/*.h -checks='*' -warnings-as-errors='*'

coverage: test_debug
	$(TEST_DEBUG)
	lcov -c -d . -o coverage.info --no-external --exclude "*/tests/*"
	genhtml coverage.info --output-directory coverage --demangle-cpp

compiledb:
	compiledb -n make

help:
	@echo "Available commands:"
	@echo "\trelease  - compile engine and tests"
	@echo "\tdebug    - compile engine and tests in debug mode"
	@echo "\tall      - release + debug"
	@echo "\tclean    - remove all files created by make"
	@echo "\tctags    - create tags for all files"
	@echo "\compiledb - create compile_commands.json


.PHONY: release lib engine test tools
.PHONY: debug lib_debug engine_debug test_debug tools_debug
.PHONY: clean compiledb ctags tidy coverage ehelp
