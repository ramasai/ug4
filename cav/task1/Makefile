CXX := g++
OPTS := -O2
LIBS := -lglut -lGLU -lGL
FRAMEWORKS := -framework GLUT -framework OpenGL

EXECUTABLE=cav
SOURCES=view.cc
OBJECTS=$(SOURCES:.cc=.o)
HEADERS=$(SOURCES:.cc=.h)

all: $(SOURCES) $(EXECUTABLE) run

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(FRAMEWORKS) $(OBJECTS) -o $@

.cc.o:
	$(CXX) $(OPTS) -c $< -o $@

clean:
	rm -f *.o cav

run:
	./cav data/arma2.obj data/skeleton2.out data/attachment2.out
