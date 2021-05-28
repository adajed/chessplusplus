include ../../makes/defines.makefile

TOOL_NAME=generate_features
RELEASE=$(BUILD_DIR)/$(TOOL_NAME).so
DEBUG=$(BUILD_DIR)/$(TOOL_NAME)_debug.so

OUTDIR_RELEASE=$(OBJS_DIR_RELEASE)/$(TOOL_NAME)
OUTDIR_DEBUG=$(OBJS_DIR_DEBUG)/$(TOOL_NAME)_debug

HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJS_RELEASE=$(SOURCES:%.cpp=$(OUTDIR_RELEASE)/%.o)
OBJS_DEBUG=$(SOURCES:%.cpp=$(OUTDIR_DEBUG)/%.o)

LIB_OBJS_RELESE=$(filter-out $(OBJS_DIR_RELEASE)/engine/main.o,$(wildcard $(OBJS_DIR_RELEASE)/engine/*.o))
LIB_OBJS_DEBUG=$(filter-out $(OBJS_DIR_DEBUG)/engine/main.o,$(wildcard $(OBJS_DIR_DEBUG)/engine/*.o))

CFLAGS_RELEASE += -fPIC
CFLAGS_DEBUG += -fPIC

LFLAGS_RELEASE +=-fPIC -shared
LFLAGS_DEBUG +=-fPIC -shared

release: $(RELEASE)

debug: $(DEBUG)

# linking
$(RELEASE): $(OBJS_RELEASE) | $(OUTDIR_RELEASE)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LIB_OBJS_RELESE) $(LFLAGS_RELEASE)

$(DEBUG): $(OBJS_DEBUG) | $(OUTDIR_DEBUG)
	@echo "Linking $@"
	@$(CXX) -o $@ $^ $(LIB_OBJS_DEBUG) $(LFLAGS_DEBUG)


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

format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_ARGS) $(SOURCES) $(HEADERS)

.PHONY: release debug format
