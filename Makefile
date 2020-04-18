OBJS = src/contrib/glad.c main.cpp #color.cpp

OBJ_NAME = main

CC = g++

COMPILER_FLAGS =

LINKER_FLAGS = -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lglfw3 -lm

INCLUDE_FLAGS = -I/home/arebel/development/tools/glfw/include -I/home/arebel/development/tools/glad/include

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_FLAGS) $(LINKER_FLAGS) -o output/$(OBJ_NAME)
	output/$(OBJ_NAME)
