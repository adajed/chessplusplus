include ./makes/defines.makefile

all: release debug

release: engine_release tests_release
debug: engine_debug tests_debug

#### engine

engine_release:
	@+make -C engine release

engine_debug:
	@+make -C engine debug


#### tests

tests_release: engine_release
	@+make -C tests release

tests_debug: engine_debug
	@+make -C tests debug


#### utils

clean:
	rm -rf $(OUTDIR)

ctags:
	ctags -R --tag-relative=yes --exclude=.git $(ROOTDIR)

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
