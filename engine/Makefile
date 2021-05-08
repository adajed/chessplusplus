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

all: release debug

release: lib_release engine_release
debug: lib_debug engine_debug

lib_release: $(LIB_RELEASE)
lib_debug: $(LIB_DEBUG)

engine_release: $(BINARY_RELEASE)
engine_debug: $(BINARY_DEBUG)

# linking
$(BINARY_RELEASE): $(OUTDIR_RELEASE)/main.o $(LIB_RELEASE) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@$(CXX) -o $@ $< $(LFLAGS_RELEASE) -L$(BUILD_DIR) -lchess_static

$(BINARY_DEBUG): $(OUTDIR_DEBUG)/main.o $(LIB_DEBUG) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@$(CXX) -o $@ $< $(LFLAGS_DEBUG) $(LCOV_LFLAGS) -L$(BUILD_DIR) -lchess_static_debug

$(LIB_RELEASE): $(filter-out $(OUTDIR_RELEASE)/main.o,$(OBJS_RELEASE)) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@$(AR) $(AR_FLAGS) $@ $^

$(LIB_DEBUG): $(filter-out $(OUTDIR_DEBUG)/main.o,$(OBJS_DEBUG)) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@$(AR) $(AR_FLAGS) $@ $^

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

