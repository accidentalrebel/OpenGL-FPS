OBJS = glad.c hello.cpp

OBJ_NAME = hello

CC = gcc

COMPILER_FLAGS =

LINKER_FLAGS = -ldl

INCLUDE_FLAGS = -I/home/arebel/development/tools/glfw/include -I/home/arebel/development/tools/glad/include

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_FLAGS) $(LINKER_FLAGS) -o output/$(OBJ_NAME)
