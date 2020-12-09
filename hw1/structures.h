#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <vector>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

using Eigen::Matrix4f;
using namespace std;

/** Contains three floats to represent a single vertex in 3D Euclidean space. Added screen_x
and screen_y in HW2 to store the pixel coordinates on screen **/
struct Vertex {
	float x;
	float y;
	float z;
	int screen_x;
	int screen_y;
	Vertex(){};
	Vertex (float x_, float y_, float z_) 
		: x(x_), y(y_), z(z_) {}
	void SetScreen(int new_x, int new_y) {
	     screen_x = new_x;
	     screen_y = new_y;
	}
};

/** Contains three integers that refer to vertices to represent a face **/
struct Face {
    int v1;
    int v2;
    int v3;
    Face(){};
    Face(int v1_, int v2_, int v3_)
        : v1(v1_), v2(v2_), v3(v3_) {}
};

/**Object struct including a vector of vertices, a vector of faces, a vector of transformation matrices  **/

struct Object {
    vector<Vertex> vertices;
    vector<Face> faces;
    vector<Matrix4f> transmatrices;
    Object() {};
    void SetVertices(vector<Vertex> new_vertices) {
         vertices = new_vertices;
    }
};

/** Orientation struct used to store orientation of our camera: x, y, z and angle **/
struct Orientation {
      float x;
      float y;
      float z;
      float angle;
      Orientation() {};
      Orientation(float x, float y, float z, float angle) 
             : x(x), y(y), z(z), angle(angle) {}
      Orientation(Orientation& o2) 
             : x(o2.x), y(o2.y), z(o2.z), angle(o2.angle) {}

};

/** Store everything (camera parameters and objects) into scene **/
struct Scene {
      Vertex position;
      Orientation orientation;
      float near;
      float far;
      float left;
      float right;
      float top;
      float bottom;
      vector<Object> objects;
      Scene() {}
      Scene(const Scene &c){
          near = c.near;
          far = c.far;
          right = c.right;
          left = c.left;
          top = c.top;
          bottom = c.bottom;
      }
      void SetObjects(vector<Object> new_objects) {
          objects = new_objects;
      } //used to update objects
      Scene& operator=(Scene& other);
      vector<Object> GetObjects() const {
          return objects;
      } //used to retrieve objects
};

#endif
