CXX := g++
OPTS := -Wall
FRAMEWORKS := -framework GLUT -framework OpenGL

EXECUTABLE=cav
SOURCES=vector3f.cpp view.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(FRAMEWORKS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(OPTS) -c $< -o $@
	
clean:
	rm -f *.o cav