include ../makes/defines.makefile

DEPDIR_RELEASE := .deps_release
DEPDIR_DEBUG := .deps_debug

DEPFLAGS_RELEASE = -MT $@ -MMD -MP -MF $(DEPDIR_RELEASE)/$*.d
DEPFLAGS_DEBUG = -MT $@ -MMD -MP -MF $(DEPDIR_DEBUG)/$*.d

BINARY_RELEASE=$(BUILD_DIR)/chessplusplus
BINARY_DEBUG=$(BUILD_DIR)/chessplusplus_debug

OUTDIR_RELEASE=$(OBJS_DIR_RELEASE)/engine
OUTDIR_DEBUG=$(OBJS_DIR_DEBUG)/engine

HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJS_RELEASE=$(SOURCES:%.cpp=$(OUTDIR_RELEASE)/%.o)
OBJS_DEBUG=$(SOURCES:%.cpp=$(OUTDIR_DEBUG)/%.o)


ifeq ($(USE_AVX2),yes)
FLAGS = -msse -msse2 -msse3 -msse4.1 -mavx2 -mpopcnt
else
FLAGS =
endif

CFLAGS_RELEASE += $(DEPFLAGS_RELEASE) $(FLAGS)
CFLAGS_DEBUG += $(DEPFLAGS_DEBUG) $(FLAGS)
LFLAGS_RELEASE += $(FLAGS)
LFLAGS_DEBUG += $(FLAGS)

all: release debug

release: engine_release
debug: engine_debug

engine_release: $(BINARY_RELEASE)
engine_debug: $(BINARY_DEBUG)

# linking
$(BINARY_RELEASE): $(OBJS_RELEASE) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@$(COMP) -o $@ $^ $(LFLAGS_RELEASE)

$(BINARY_DEBUG): $(OBJS_DEBUG) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@$(COMP) -o $@ $^ $(LFLAGS_DEBUG) $(LCOV_LFLAGS)

# compilation
$(OUTDIR_RELEASE)/%.o : %.cpp $(DEPDIR_RELEASE)/%.d | $(OUTDIR_RELEASE) $(DEPDIR_RELEASE)
	@echo "Compiling release $<"
	@$(COMP) -c -o $@ $< $(CFLAGS_RELEASE)

$(OUTDIR_DEBUG)/%.o : %.cpp $(DEPDIR_DEBUG)/%.d | $(OUTDIR_DEBUG) $(DEPDIR_DEBUG)
	@echo "Compiling debug $<"
	@$(COMP) -c -o $@ $< $(CFLAGS_DEBUG)

# dir
$(OUTDIR_RELEASE):
	mkdir -p $@

$(OUTDIR_DEBUG):
	mkdir -p $@

$(DEPDIR_RELEASE):
	mkdir -p $@

$(DEPDIR_DEBUG):
	mkdir -p $@

DEPFILES_RELEASE := $(SOURCES:%.cpp=$(DEPDIR_RELEASE)/%.d)
$(DEPFILES_RELEASE):

DEPFILES_DEBUG := $(SOURCES:%.cpp=$(DEPDIR_DEBUG)/%.d)
$(DEPFILES_DEBUG):

include $(wildcard $(DEPFILES_RELEASE))
include $(wildcard $(DEPFILES_DEBUG))

format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_ARGS) $(SOURCES) $(HEADERS)

.PHONY: format release debug engine_release engine_debug
