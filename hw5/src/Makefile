CC = g++
FLAGS = -g

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
INCLUDE = -isystem../ -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
SOURCES = *.cpp
LIBS = -lGLEW -lGL -lGLU -lglut -lm

EXENAME = opengl

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(LIBDIR) $(SOURCES) $(LIBS)

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean

