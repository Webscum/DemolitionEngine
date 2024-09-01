# A simple Makefile for compiling small SDL projects

# set the compiler
CC := gcc

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lm -lSDL2_mixer
# add header files here
HDRS :=

# add source files here
SRCS := main.c

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := renderer

# default recipe
all: $(EXEC)
 
showfont: showfont.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS)

glfont: glfont.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS)

# recipe for building the final executable
$(EXEC): $(SRCS) $(HDRS) Makefile
	$(CC) $(SRCS) -o $@ $(CFLAGS)

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) Makefile
#    $(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean

force: 
	touch main.c
	make all