# COMPILER
CC = g++-6

# SFML
SFML_INCLUDE = $(SFML_HOME)/include
SFML_LIB = $(SFML_HOME)/lib

# INPUT
IN_FILES = *.cpp

LIBRARIES = -lsfml-graphics -lsfml-window -lsfml-system -lm -lGL -lpthread

# EXECUTABLE NAME
OUT_FILE = nbody2

#COMPILER FLAGS
FLAGS = -std=c++14 -fpermissive -fopenmp -pthread

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


