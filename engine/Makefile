include ../makes/defines.makefile

CPP_SOURCES=$(wildcard *.cpp)
CPP_HEADERS=$(wildcard *.h)

OBJS_RELEASE=$(patsubst %.cpp,$(OUTDIR_ENGINE)/%.r.o,$(CPP_SOURCES))
OBJS_DEBUG  =$(patsubst %.cpp,$(OUTDIR_ENGINE)/%.d.o,$(CPP_SOURCES))

release: $(LIB_STATIC_RELEASE)
debug: $(LIB_STATIC_DEBUG)


#### linking

$(LIB_STATIC_RELEASE): $(OBJS_RELEASE)
	@echo "Linking $@"
	@$(AR) rs $@ $^

$(LIB_STATIC_DEBUG): $(OBJS_DEBUG)
	@echo "Linking $@"
	@$(AR) rs $@ $^


#### compling

$(OUTDIR_ENGINE)/%.r.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_ENGINE)
	@echo "Compiling release $<"
	@$(CXX) -c -o $@ $(CFLAGS_RELEASE) $<

$(OUTDIR_ENGINE)/%.d.o: %.cpp $(CPP_HEADERS) | $(OUTDIR_ENGINE)
	@echo "Compiling debug $<"
	@$(CXX) -c -o $@ $(CFLAGS_DEBUG) $<

$(OUTDIR_ENGINE):
	@mkdir -p $@
