CC = g++
DEBUG =
CFLAGS = -pthread -c $(DEBUG)
LDFLAGS = -pthread $(DEBUG)
SOURCES = beamforming.cpp microphone.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = benchmark

all: $(EXECUTABLE)

debug: DEBUG+=-g
debug: clean $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
    
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
