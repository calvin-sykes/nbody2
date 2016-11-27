# COMPILER
CC = g++-6

# SFML
SFML_INCLUDE = $(SFML_HOME)/include
SFML_LIB = $(SFML_HOME)/lib

# INPUT
IN_FILES = main.cpp AssetManager.cpp BodyTrail.cpp imgui_draw.cpp Quad.cpp StartState.cpp BHTree.cpp imgui.cpp imgui_sfml.cpp RunState.cpp Body2d.cpp imgui_demo.cpp Sim.cpp

LIBRARIES = -lsfml-graphics -lsfml-window -lsfml-system -lm -lGL -lpthread

# EXECUTABLE NAME
OUT_FILE = nbody2

#COMPILER FLAGS
FLAGS = -fpermissive -fopenmp -pthread

# COMPILER COMMAND
BUILD_CMD = $(IN_FILES) -o $(OUT_FILE) -I$(SFML_INCLUDE) -L$(SFML_LIB) $(LIBRARIES)

build:
	cd nbody2; \
	$(CC) $(FLAGS) $(BUILD_CMD)

debug:
	cd nbody2; \
	$(CC) -ggdb -O0 $(FLAGS) $(BUILD_CMD)
	
clean:
	cd nbody2 && rm *.o
