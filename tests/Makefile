include ../makes/defines.makefile

PROGRAM_RELEASE=$(OUTDIR)/tests
PROGRAM_DEBUG=$(OUTDIR)/tests_debug

CPP_SOURCES=$(wildcard *.cpp)
CPP_HEADERS=$(wildcard *.h)

OBJS_RELEASE=$(patsubst %.cpp,$(OUTDIR_TESTS)/%.r.o,$(CPP_SOURCES))
OBJS_DEBUG=$(patsubst %.cpp,$(OUTDIR_TESTS)/%.d.o,$(CPP_SOURCES))
OBJS_ENGINE_RELEASE=$(filter-out $(OUTDIR_ENGINE)/main.r.o $(OUTDIR_ENGINE)/perft.r.o,$(wildcard $(OUTDIR_ENGINE)/*.r.o))
OBJS_ENGINE_DEBUG  =$(filter-out $(OUTDIR_ENGINE)/main.d.o $(OUTDIR_ENGINE)/perft.d.o,$(wildcard $(OUTDIR_ENGINE)/*.d.o))

CFLAGS_RELEASE += -I$(ROOTDIR)/engine
CFLAGS_DEBUG += -I$(ROOTDIR)/engine

LFLAGS_COMMON += -L$(GTEST_LIB_PATH) -lgtest -pthread

release: $(PROGRAM_RELEASE)
debug: $(PROGRAM_DEBUG)


#### linking

$(PROGRAM_RELEASE): $(OBJS_RELEASE)
	@echo "Linking $@"
	@$(CC) $^ $(OBJS_ENGINE_RELEASE) -o $@ $(LFLAGS_COMMON)

$(PROGRAM_DEBUG): $(OBJS_DEBUG)
	@echo "Linking $@"
	@$(CC) $^ $(OBJS_ENGINE_DEBUG) -o $@ $(LFLAGS_COMMON)


#### compling

$(OUTDIR_TESTS)/%.r.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_TESTS)
	@echo "Compiling release $<"
	@$(CC) -c -o $@ $(CFLAGS_RELEASE) $<

$(OUTDIR_TESTS)/%.d.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_TESTS)
	@echo "Compiling debug $<"
	@$(CC) -c -o $@ $(CFLAGS_DEBUG) $<

$(OUTDIR_TESTS):
	mkdir -p $@
