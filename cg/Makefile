CXX := g++
LIBS := -lglut -lGLU -lGL
OPTIONS := -O3

# Linking
viewer: viewer.o
	$(CXX) $(OPTS) $(LIBS) viewer.o -o viewer

%.o: %.cc
	$(CXX) $(OPTS) $(LIBS) -c $< -o $@

clean:
	rm -f *.o viewer
