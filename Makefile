# COMPILER
CC = g++-6

# SFML
SFML_INCLUDE = $(SFML_HOME)/include
SFML_LIB = $(SFML_HOME)/lib

# INPUT
IN_FILES = main.cpp AssetManager.cpp BodyTrail.cpp imgui_draw.cpp Quad.cpp StartState.cpp BHTree.cpp imgui.cpp imgui-SFML.cpp RunState.cpp Body2d.cpp imgui_demo.cpp Sim.cpp

LIBRARIES = -lsfml-graphics -lsfml-window -lsfml-system -lm -lGL

# EXECUTABLE NAME
OUT_FILE = nbody2

# COMPILER COMMAND
BUILD_CMD = $(CC) $(IN_FILES) -o $(OUT_FILE) -I$(SFML_INCLUDE) -L$(SFML_LIB) $(LIBRARIES)

build:
	cd nbody2; \
	$(BUILD_CMD);
