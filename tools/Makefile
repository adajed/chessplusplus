include ../makes/defines.makefile

.PHONY: release debug format
.PHONY: regression regression_debug
.PHONY: score_eval score_eval_debug
.PHONY: generate_features

release: regression score_eval generate_features

debug: regression_debug score_eval_debug

# regression
regression:
	@+make -C regression release

regression_debug:
	@+make -C regression debug


# score_eval
score_eval:
	@+make -C score_eval release

# score_eval
score_eval_debug:
	@+make -C score_eval debug

generate_features:
	@+make -C generate_features release

format:
	@+make -C regression format
	@+make -C score_eval format
