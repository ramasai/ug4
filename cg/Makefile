CXX := g++
LIBS := -lglut -lGLU -lGL

# Linking
viewer: viewer.o
	$(CXX) $(LIBS) viewer.o -o viewer

%.o: %.cc
	$(CXX) $(LIBS) -c $< -o $@
