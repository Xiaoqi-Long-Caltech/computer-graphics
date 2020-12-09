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

using namespace Eigen;
using namespace std;

/** Color struct: RGB values **/
struct Color {
    float r;
    float g;
    float b;
    Color() {};
    Color(float r, float g, float b) : r(r), g(g), b(b) {};
    Vector3f GetColor() {
        Vector3f color(r, g, b);
        return color;
    }
};

/** Material parameters **/
struct Material {
    Color ambient;
    Color diffuse;
    Color specular;
    float shininess;
};

/** Contains three floats to represent a single vertex in 3D Euclidean space. Added screen_x
and screen_y in HW1 to store the pixel coordinates on screen. Added world_x, y, z in HW2 to store world coordinates to use in Phong algorithm **/
struct Vertex {
	float x;
	float y;
	float z;
	float world_x;
	float world_y;
	float world_z;
	float screen_x;
	float screen_y;
	Vertex(){};
	Vertex (float x_, float y_, float z_)
	        : x(x_), y(y_), z(z_) {}
	Vertex (float x_, float y_, float z_, float world_x_, float world_y_, float world_z_, int screen_x_, int screen_y_) 
		: x(x_), y(y_), z(z_), world_x(world_x_), world_y(world_y_), world_z(world_z_), screen_x(screen_x_), screen_y(screen_y_) {}
	void SetXYZ(float new_x, float new_y, float new_z) {
	     x = new_x;
	     y = new_y;
	     z = new_z;
	}
	void SetScreen(int new_x, int new_y) {
	     screen_x = new_x;
	     screen_y = new_y;
	}
	void SetWorld(float new_x, float new_y, float new_z) {
	     world_x = new_x;
	     world_y = new_y;
	     world_z = new_z;
	}
	Vector3f GetCoords(){
	     Vector3f coords(x, y, z);
	     return coords;
	}
	Vector3f GetWorld(){
	     Vector3f world(world_x, world_y, world_z);
	     return world;
	}
};

/**Contains the x, y, z components of a surface normal of the object. **/
struct Normal {
       float x;
       float y;
       float z;
       Normal() {}
       Normal (float x_, float y_, float z_) 
		: x(x_), y(y_), z(z_) {}
       Vector3f GetNormal(){
           Vector3f normal(x, y, z);
           return normal;
       }
       void SetX(float new_x) {
           x = new_x;
       }
       void SetY(float new_y) {
           y = new_y;
       }
       void SetZ(float new_z) {
           z = new_z;
       }
};

/** Contains three integers that refer to vertices to represent a face, and also three integers that represent the vertices of the surface normal to the face. **/
struct Face {
    int v1;
    int v2;
    int v3;
    int vn1;
    int vn2;
    int vn3;
    Color c1;
    Color c2;
    Color c3;
    Face(){};
    Face(int v1_, int v2_, int v3_, int vn1_, int vn2_, int vn3_)
        : v1(v1_), v2(v2_), v3(v3_), vn1(vn1_), vn2(vn2_), vn3(vn3_) {}
    void SetC1(Color new_c1) {
        c1 = new_c1;
    }
    void SetC2(Color new_c2) {
        c2 = new_c2;
    }
    void SetC3(Color new_c3) {
        c3 = new_c3;
    }
};

/**Light struct contains position, color, and attenuation parameter**/
struct Light{
    Vertex position;
    Color c;
    float k;
    Light(float x, float y, float z, float r, float g, float b, float k_){
    	Vertex ver(x, y, z);
    	position = ver;
    	c = Color(r, g, b);
    	k = k_;
    }
};

/**Object struct including a vector of vertices, a vector of faces, a vector of transformation matrices, normals, transformation matrices of the normals, and material  **/

struct Object {
    vector<Vertex> vertices;
    vector<Face> faces;
    vector<Matrix4f> transmatrices;
    vector<Normal> normals;
    vector<Matrix4f> normal_trans;
    Material material;
    Object() {};
    void SetVertices(vector<Vertex> new_vertices) {
         vertices = new_vertices;
    }
    void SetNormals(vector<Normal> new_normals) {
         normals = new_normals;
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
      vector<Light> lights;
      MatrixXf depth_buffer;
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
      void SetDepth(float y, float x, float number) {
          depth_buffer(y, x) = number;
      }
      Scene& operator=(Scene& other);
      vector<Object> GetObjects() const {
          return objects;
      } //used to retrieve objects
      void SetLights(vector<Light> new_lights) {
      	   lights = new_lights;
      }
      vector<Light> GetLights() const {
      	  return lights;
      }
};

//used for grid of colors, dynamic size depends on xres and yres
typedef Matrix<Color, Dynamic, Dynamic> MatrixColor; 

#endif
