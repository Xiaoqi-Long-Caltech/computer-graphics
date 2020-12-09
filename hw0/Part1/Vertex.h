#ifndef VERTEX_H
#define VERTEX_H


/** Contains three floats to represent a single vertex in 3D Euclidean space **/
struct Vertex {
	float x;
	float y;
	float z;
	Vertex (float x_, float y_, float z_) 
		: x(x_), y(y_), z(z_) {};
};

#endif
