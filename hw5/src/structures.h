#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "structs.h"
#include "halfedge.h"
#define _USE_MATH_DEFINES

using namespace std;
using namespace Eigen;

const float EPSILON = 0.00001;

struct Point_Light
{
    float position[4];
    float color[3];
    float attenuation_k;
    Point_Light(float x, float y, float z, float w,
            float r, float g, float b, float k) {
        position[0] = x;
        position[1] = y;
        position[2] = z;
        position[3] = w;
        color[0] = r;
        color[1] = g;
        color[2] = b;
        attenuation_k = k;
    }
    Point_Light() {}
};

struct Transforms
{
    float translation[3];
    float rotation[3];
    float scaling[3];

    float rotation_angle;
    
    /* When we parse a Transform, with each type of transform, we parse in the other types of transforms as identities */
    Transforms(string t, float x, float y, float z) {
        if (t.compare("t") == 0) {
           translation[0] = x;
           translation[1] = y;
           translation[2] = z;
           scaling[0] = 1;
           scaling[1] = 1;
           scaling[2] = 1;
           rotation[0] = 1;
           rotation[1] = 0;
           rotation[2] = 0;
           rotation_angle = 0;
        } else if (t.compare("s") == 0) {
           scaling[0] = x;
           scaling[1] = y;
           scaling[2] = z;
           translation[0] = 0;
           translation[1] = 0;
           translation[2] = 0;
           rotation[0] = 1;
           rotation[1] = 0;
           rotation[2] = 0;
           rotation_angle = 0;
        }
    }
    
    /* Rotation transform needs an additional function argument */
    Transforms(string t, float x, float y, float z, float angle) {
        if (t.compare("r") == 0) {
           rotation[0] = x;
           rotation[1] = y;
           rotation[2] = z;
           rotation_angle = angle;
           translation[0] = 0;
           translation[1] = 0;
           translation[2] = 0;
           scaling[0] = 1;
           scaling[1] = 1;
           scaling[2] = 1;
        }
    }
};


struct Object
{
    vector<Vertex> vertex_buffer;
    vector<Vec3f> normal_buffer;

    vector<Transforms> transform_sets;
    
    vector<Face> faces;
    vector<Vertex> vertex_sets;
    
    vector<Vec3f> normal_sets;

    float ambient_reflect[3];
    float diffuse_reflect[3];
    float specular_reflect[3];

    float shininess;
    
    /* constructors */
    Object() {};
    void SetAmbient(float r, float g, float b) {
        ambient_reflect[0] = r;
        ambient_reflect[1] = g;
        ambient_reflect[2] = b;
    }

    void SetDiffuse(float r, float g, float b) {
        diffuse_reflect[0] = r;
        diffuse_reflect[1] = g;
        diffuse_reflect[2] = b;
    }

    void SetSpecular(float r, float g, float b) {
        specular_reflect[0] = r;
        specular_reflect[1] = g;
        specular_reflect[2] = b;
    }
};

/* Scene struct as in last assignment */
struct Scene {
    Vec3f position_camera;
    Vec3f orientation_camera;
    float orientation_angle_cam;
    float near;
    float far;
    float left;
    float right;
    float top;
    float bottom;
    vector<Point_Light> lights;
    vector<Object> objects;
    vector<Object> smooth_objects;
    vector<Mesh_Data *> meshes;
    Mesh_Data *mesh_data;
    vector<HEV*> *hevs;
    vector<vector<HEF *>*> hefs;
    int xres;
    int yres;
    float h; //time step
    Scene() {}
    
};

#endif
