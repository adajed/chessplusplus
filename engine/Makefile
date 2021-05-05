include ../makes/defines.makefile

BINARY_RELEASE=$(BUILD_DIR)/chessplusplus
BINARY_DEBUG=$(BUILD_DIR)/chessplusplus_debug

LIB_RELEASE=$(BUILD_DIR)/libchess_static.a
LIB_DEBUG=$(BUILD_DIR)/libchess_static_debug.a

OUTDIR_RELEASE=$(OBJS_DIR_RELEASE)/engine
OUTDIR_DEBUG=$(OBJS_DIR_RELEASE)/engine_debug

SOURCES=$(wildcard *.cpp)
OBJS_RELEASE=$(SOURCES:%.cpp=$(OUTDIR_RELEASE)/%.o)
OBJS_DEBUG=$(SOURCES:%.cpp=$(OUTDIR_DEBUG)/%.o)

EXPORTS_MAP=exports.map

all: release debug

release: $(BINARY_RELEASE) $(LIB_RELEASE)
debug: $(BINARY_DEBUG) $(LIB_DEBUG)

# linking
$(BINARY_RELEASE): $(OBJS_RELEASE) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS_RELEASE)

$(BINARY_DEBUG): $(OBJS_DEBUG) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LFLAGS_DEBUG) $(LCOV_LFLAGS)

$(LIB_RELEASE): $(filter-out $(OUTDIR_RELEASE)/main.o,$(OBJS_RELEASE)) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@ar rs $@ $^

$(LIB_DEBUG): $(filter-out $(OUTDIR_DEBUG)/main.o,$(OBJS_DEBUG)) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@ar rs $@ $^

# compilation
$(OUTDIR_RELEASE)/%.o : %.cpp | $(OUTDIR_RELEASE)
	@echo "Compiling release $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_RELEASE)

$(OUTDIR_DEBUG)/%.o : %.cpp | $(OUTDIR_DEBUG)
	@echo "Compiling debug $<"
	@$(CXX) -c -o $@ $< $(CFLAGS_DEBUG)

# dir
$(OUTDIR_RELEASE):
	mkdir -p $@

$(OUTDIR_DEBUG):
	mkdir -p $@
