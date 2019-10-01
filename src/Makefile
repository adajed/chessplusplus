all: release debug

release: engine
debug: engine_debug

CPP_SOURCES=$(wildcard *.cpp)
CPP_INCLUDES=$(wildcard *.h)
OBJS_RELEASE=$(patsubst %.cpp,%.o,$(CPP_SOURCES))
OBJS_DEBUG=$(patsubst %.cpp,%.d.o,$(CPP_SOURCES))


CFLAGS_RELEASE = -Wall -O3 -Wno-array-bounds
CFLAGS_DEBUG = -Wall -g -Wno-array-bounds

engine: $(OBJS_RELEASE)
	g++ -o $@ $(OBJS_RELEASE) $(CFLAGS_RELEASE)

engine_debug: $(OBJS_DEBUG)
	g++ -o $@ $(OBJS_DEBUG) $(CFLAGS_DEBUG)

%.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS_RELEASE)

%.d.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS_DEBUG)

clean:
	rm -f engine engine_debug *.o
