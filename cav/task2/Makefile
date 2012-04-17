CXX := g++
OPTS := -O2
LIBS := -lglut -lGLU -lGL
FRAMEWORKS := -framework GLUT -framework OpenGL

EXECUTABLE=main
SOURCES=main.cc
OBJECTS=$(SOURCES:.cc=.o)
HEADERS=$(SOURCES:.cc=.h)

all: $(SOURCES) $(EXECUTABLE) run

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LIBS) $(OBJECTS) -o $@

.cc.o:
	$(CXX) $(OPTS) -c $< -o $@

clean:
	rm -f *.o main

run:
	./main head
