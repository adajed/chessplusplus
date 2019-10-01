include ../makes/defines.makefile

PROGRAM_RELEASE=$(OUTDIR)/engine
PROGRAM_DEBUG=$(OUTDIR)/engine_debug

CPP_SOURCES=$(wildcard *.cpp)
CPP_HEADERS=$(wildcard *.h)

OBJS_RELEASE=$(patsubst %.cpp,$(OUTDIR_ENGINE)/%.r.o,$(CPP_SOURCES))
OBJS_DEBUG=$(patsubst %.cpp,$(OUTDIR_ENGINE)/%.d.o,$(CPP_SOURCES))

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

$(OUTDIR_ENGINE)/%.r.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_ENGINE)
	@echo "Compiling release $<"
	@$(CC) -c -o $@ $(CFLAGS_RELEASE) $<

$(OUTDIR_ENGINE)/%.d.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_ENGINE)
	@echo "Compiling debug $<"
	@$(CC) -c -o $@ $(CFLAGS_DEBUG) $<

$(OUTDIR_ENGINE):
	@mkdir -p $@
