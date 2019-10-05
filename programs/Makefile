include ../makes/defines.makefile

ENGINE_RELEASE=$(OUTDIR)/engine
ENGINE_DEBUG=$(OUTDIR)/engine_debug
ENGINE_OBJ_RELEASE=$(OUTDIR_PROGRAMS)/engine.r.o
ENGINE_OBJ_DEBUG=$(OUTDIR_PROGRAMS)/engine.d.o

PERFT_RELEASE=$(OUTDIR)/perft
PERFT_DEBUG=$(OUTDIR)/perft_debug
PERFT_OBJ_RELEASE=$(OUTDIR_PROGRAMS)/perft.r.o
PERFT_OBJ_DEBUG=$(OUTDIR_PROGRAMS)/perft.d.o

PLAY_RELEASE=$(OUTDIR)/play
PLAY_DEBUG=$(OUTDIR)/play_debug
PLAY_OBJ_RELEASE=$(OUTDIR_PROGRAMS)/play.r.o
PLAY_OBJ_DEBUG=$(OUTDIR_PROGRAMS)/play.d.o


CPP_SOURCES=$(wildcard *.cpp)
CPP_HEADERS=$(wildcard *.h) $(wildcard ../engine/*.h)

release: $(ENGINE_RELEASE) $(PERFT_RELEASE) $(PLAY_RELEASE)
debug: $(ENGINE_DEBUG) $(PERFT_DEBUG) $(PLAY_DEBUG)


#### linking

$(ENGINE_RELEASE): $(ENGINE_OBJ_RELEASE)
	@echo "Linking $@"
	@$(CC) $^ $(LIB_STATIC_RELEASE) -o $@ $(LFLAGS_RELEASE)

$(ENGINE_DEBUG): $(ENGINE_OBJ_DEBUG)
	@echo "Linking $@"
	@$(CC) $^ $(LIB_STATIC_DEBUG) -o $@ $(LFLAGS_DEBUG)

$(PERFT_RELEASE): $(PERFT_OBJ_RELEASE)
	@echo "Linking $@"
	@$(CC) $^ $(LIB_STATIC_RELEASE) -o $@ $(LFLAGS_RELEASE)

$(PERFT_DEBUG): $(PERFT_OBJ_DEBUG)
	@echo "Linking $@"
	@$(CC) $^ $(LIB_STATIC_DEBUG) -o $@ $(LFLAGS_DEBUG)

$(PLAY_RELEASE): $(PLAY_OBJ_RELEASE)
	@echo "Linking $@"
	@$(CC) $^ $(LIB_STATIC_RELEASE) -o $@ $(LFLAGS_RELEASE)

$(PLAY_DEBUG): $(PLAY_OBJ_DEBUG)
	@echo "Linking $@"
	@$(CC) $^ $(LIB_STATIC_DEBUG) -o $@ $(LFLAGS_DEBUG)


#### compling

$(OUTDIR_PROGRAMS)/%.r.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_PROGRAMS)
	@echo "Compiling release $<"
	@$(CC) -c -o $@ $(CFLAGS_RELEASE) $<

$(OUTDIR_PROGRAMS)/%.d.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_PROGRAMS)
	@echo "Compiling debug $<"
	@$(CC) -c -o $@ $(CFLAGS_DEBUG) $<

$(OUTDIR_PROGRAMS):
	@mkdir -p $@
