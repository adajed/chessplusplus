include ../makes/defines.makefile

.PHONY: release debug regression regression_debug format

release: regression

debug: regression_debug

# regression
regression:
	@+make -C regression release

regression_debug:
	@+make -C regression debug

format:
	@+make -C regression format
