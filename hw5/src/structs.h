#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>

struct Vec3f
{
	float x, y, z;
	Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vec3f() {}
};

struct Vertex
{
    float x, y, z;
    Vertex(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vertex() {}
};

struct Face
{
    int idx1, idx2, idx3;
    Face(float idx1_, float idx2_, float idx3_) : idx1(idx1_), idx2(idx2_),
            idx3(idx3_) {}
};

struct Mesh_Data
{
    std::vector<Vertex*> *vertices;
    std::vector<Face*> *faces;
};

#endif
