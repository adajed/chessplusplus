include ./makes/defines.makefile

all: release debug

release: engine test tools
debug: engine_debug test_debug tools_debug

engine:
	@+make -C engine engine_release

engine_debug:
	@+make -C engine engine_debug


#### test

test: engine
	@+make -C tests release

test_debug: engine_debug
	@+make -C tests debug


#### tools

tools: engine
	@+make -C tools release

tools_debug: engine_debug
	@+make -C tools debug

#### utils

clean:
	$(RM) -r $(BUILD_DIR)

ctags:
	ctags -R --tag-relative=yes --exclude=.git $(ROOTDIR)

format:
	@+make -C engine format
	@+make -C tools format

tidy:
	@+make -C engine tidy
	@+make -C tools tidy

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


.PHONY: release engine test tools
.PHONY: debug engine_debug test_debug tools_debug
.PHONY: clean compiledb ctags tidy format coverage ehelp
