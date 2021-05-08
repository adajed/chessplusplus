include ../makes/defines.makefile

RELEASE=$(BUILD_DIR)/tests
DEBUG=$(BUILD_DIR)/tests_debug

OUTDIR_RELEASE=$(OBJS_DIR_RELEASE)/tests
OUTDIR_DEBUG=$(OBJS_DIR_DEBUG)/tests

SOURCES=$(wildcard *.cpp)
OBJS_RELEASE=$(SOURCES:%.cpp=$(OUTDIR_RELEASE)/%.o)
OBJS_DEBUG=$(SOURCES:%.cpp=$(OUTDIR_DEBUG)/%.o)

all: release debug

release: $(RELEASE)
debug: $(DEBUG)

LIB_OBJS_RELESE=$(filter-out $(OBJS_DIR_RELEASE)/engine/main.o,$(wildcard $(OBJS_DIR_RELEASE)/engine/*.o))
LIB_OBJS_DEBUG=$(filter-out $(OBJS_DIR_DEBUG)/engine/main.o,$(wildcard $(OBJS_DIR_DEBUG)/engine/*.o))

# linking
$(RELEASE): $(OBJS_RELEASE) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LIB_OBJS_RELESE) $(LFLAGS_RELEASE) -L$(GTEST_LIB_PATH) -lgtest

$(DEBUG): $(OBJS_DEBUG) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LIB_OBJS_DEBUG) $(LFLAGS_DEBUG) $(LCOV_LFLAGS) -L$(GTEST_LIB_PATH) -lgtest


# compilation
$(OUTDIR_RELEASE)/%.o : %.cpp | $(OUTDIR_RELEASE)
	@echo "Compiling release $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_RELEASE) -I$(ROOTDIR)/engine

$(OUTDIR_DEBUG)/%.o : %.cpp | $(OUTDIR_DEBUG)
	@echo "Compiling debug $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_DEBUG) -I$(ROOTDIR)/engine

# dir
$(OUTDIR_RELEASE):
	mkdir -p $@

$(OUTDIR_DEBUG):
	mkdir -p $@

