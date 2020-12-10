#ifndef OPENGL_H
#define OPENGL_H
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#define _USE_MATH_DEFINES
#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <cstdlib>
#include "arcball.h"
using namespace std;

/* initialize variables and functions, copied from OpenGL_demo.cpp */

void init(void);
void reshape(int width, int height);
void display(void);

void init_lights();
void set_lights();
void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

float deg2rad(float);
float rad2deg(float);


#endif
