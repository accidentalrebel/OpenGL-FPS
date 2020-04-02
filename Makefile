OBJS = glad.c hello.cpp
OBJ_NAME = hello
all: $(OBJS)
	gcc $(OBJS) -I/home/arebel/development/tools/glfw/include -I/home/arebel/development/tools/glad/include -ldl -o output/$(OBJ_NAME)
