include ../makes/defines.makefile

ENGINE_RELEASE=$(OUTDIR)/engine
ENGINE_DEBUG=$(OUTDIR)/engine_debug

PERFT_RELEASE=$(OUTDIR)/perft
PERFT_DEBUG=$(OUTDIR)/perft_debug

CPP_SOURCES=$(wildcard *.cpp)
CPP_HEADERS=$(wildcard *.h)

OBJS_RELEASE=$(patsubst %.cpp,$(OUTDIR_ENGINE)/%.r.o,$(CPP_SOURCES))
OBJS_DEBUG  =$(patsubst %.cpp,$(OUTDIR_ENGINE)/%.d.o,$(CPP_SOURCES))

OBJS_ENGINE_RELEASE=$(filter-out $(OUTDIR_ENGINE)/perft.r.o,$(OBJS_RELEASE))
OBJS_ENGINE_DEBUG  =$(filter-out $(OUTDIR_ENGINE)/perft.d.o,$(OBJS_DEBUG))

OBJS_PERFT_RELEASE=$(filter-out $(OUTDIR_ENGINE)/main.r.o,$(OBJS_RELEASE))
OBJS_PERFT_DEBUG  =$(filter-out $(OUTDIR_ENGINE)/main.d.o,$(OBJS_DEBUG))

release: $(ENGINE_RELEASE) $(PERFT_RELEASE)
debug: $(ENGINE_DEBUG) $(PERFT_DEBUG)

#### linking

$(ENGINE_RELEASE): $(OBJS_ENGINE_RELEASE)
	@echo "Linking $@"
	@$(CC) $^ -o $@ $(LFLAGS_COMMON)

$(ENGINE_DEBUG): $(OBJS_ENGINE_DEBUG)
	@echo "Linking $@"
	@$(CC) $^ -o $@ $(LFLAGS_COMMON)

$(PERFT_RELEASE): $(OBJS_PERFT_RELEASE)
	@echo "Linking $@"
	@$(CC) $^ -o $@ $(LFLAGS_COMMON)

$(PERFT_DEBUG): $(OBJS_PERFT_DEBUG)
	@echo "Linking $@"
	@$(CC) $^ -o $@ $(LFLAGS_COMMON)


#### compling

$(OUTDIR_ENGINE)/%.r.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_ENGINE)
	@echo "Compiling release $<"
	@$(CC) -c -o $@ $(CFLAGS_RELEASE) $<

$(OUTDIR_ENGINE)/%.d.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_ENGINE)
	@echo "Compiling debug $<"
	@$(CC) -c -o $@ $(CFLAGS_DEBUG) $<

$(OUTDIR_ENGINE):
	@mkdir -p $@
