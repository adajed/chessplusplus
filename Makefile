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

coverage: test_debug
	$(TEST_DEBUG)
	lcov -c -d . -o coverage.info --no-external --exclude "*/tests/*"
	genhtml coverage.info --output-directory coverage --demangle-cpp

compiledb:
	compiledb -n make

help:
	@echo "Available commands:"
	@echo "\trelease   - compile engine and tests"
	@echo "\tdebug     - compile engine and tests in debug mode"
	@echo "\tall       - release + debug"
	@echo "\tclean     - remove all files created by make"
	@echo "\tctags     - create tags for all files"
	@echo "\tformat    - run clang-format on all files"
	@echo "\tcompiledb - create compile_commands.json"
	@echo "\tinfo      - print compile info"

info:
	@echo "Chess engine chessplusplus"
	@echo ""
	@echo "Compilation:"
	@echo "make COMP=<path to C++ compiler>"
	@echo "Default COMP is g++."
	@echo ""
	@echo "Examples:"
	@echo "make COMP=g++"
	@echo "make COMP=clang++"


.PHONY: release engine test tools
.PHONY: debug engine_debug test_debug tools_debug
.PHONY: clean compiledb ctags format coverage help info
