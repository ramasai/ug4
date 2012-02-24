CXX := g++
OPTS := -Wall
LIBS := -lglut -lGLU -lGL

EXECUTABLE=cav
SOURCES=view.cc
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LIBS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(OPTS) -c $< -o $@
	
clean:
	rm -f *.o cav
