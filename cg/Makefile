CXX := g++
LIBS := -lglut -lGLU -lGL

# Linking
viewer: viewer.o
	$(CXX) $(LIBS) -o viewer

%.o: %.cc
	$(CXX) $(LIBS) -c $(input) -o $(output)
