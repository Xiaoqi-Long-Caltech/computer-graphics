
CC = g++
FLAGS = -std=c++14 -g -o

INCLUDE = -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
SOURCES = opengl.cpp
LIBS = -lGLEW -lGL -lGLU -lglut -lm

EXENAME = opengl

all: $(SOURCES) *.h
	$(CC) $(FLAGS) $(EXENAME) $(INCLUDE) $(LIBDIR) $(SOURCES) $(LIBS)

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean
