CC = g++
DEBUG =
CFLAGS = -pthread -std=c++0x -c $(DEBUG)
LDFLAGS = -pthread -std=c++0x $(DEBUG)
SOURCES = beamforming.cpp microphone.cpp wav-file.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = run

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
