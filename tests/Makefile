include ../makes/defines.makefile

PROGRAM_RELEASE=$(OUTDIR)/tests
PROGRAM_DEBUG=$(OUTDIR)/tests_debug

CPP_SOURCES=$(wildcard *.cpp)
CPP_HEADERS=$(wildcard *.h)

OBJS_RELEASE=$(patsubst %.cpp,$(OUTDIR_TESTS)/%.r.o,$(CPP_SOURCES))
OBJS_DEBUG=$(patsubst %.cpp,$(OUTDIR_TESTS)/%.d.o,$(CPP_SOURCES))

CFLAGS_RELEASE += -I$(ROOTDIR)/engine -I$(GTEST_INCLUDE_PATH)
CFLAGS_DEBUG += -I$(ROOTDIR)/engine -I$(GTEST_INCLUDE_PATH)

LFLAGS_COMMON += -L$(GTEST_LIB_PATH) -lgtest -pthread

release: $(PROGRAM_RELEASE)
debug: $(PROGRAM_DEBUG)


#### linking

$(PROGRAM_RELEASE): $(OBJS_RELEASE)
	@echo "Linking $@"
	@$(CXX) $^ $(OBJS_ENGINE_RELEASE) -o $@ $(LIB_STATIC_RELEASE) $(LFLAGS_COMMON)

$(PROGRAM_DEBUG): $(OBJS_DEBUG)
	@echo "Linking $@"
	@$(CXX) $^ $(OBJS_ENGINE_DEBUG) -o $@ $(LIB_STATIC_DEBUG) $(LFLAGS_COMMON)


#### compling

$(OUTDIR_TESTS)/%.r.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_TESTS)
	@echo "Compiling release $<"
	@$(CXX) -c -o $@ $(CFLAGS_RELEASE) $<

$(OUTDIR_TESTS)/%.d.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_TESTS)
	@echo "Compiling debug $<"
	@$(CXX) -c -o $@ $(CFLAGS_DEBUG) $<

$(OUTDIR_TESTS):
	mkdir -p $@
