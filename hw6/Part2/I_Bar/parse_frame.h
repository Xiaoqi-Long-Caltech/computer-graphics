#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

struct Vec3f
{
	float x, y, z;
	Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vec3f() {}
};

struct Vec4f
{
	float x, y, z, a;
	Vec4f(float x_, float y_, float z_, float a_) : x(x_), y(y_), z(z_), a(a_) {}
	Vec4f() {}
};

// store keyframe information
struct Frame {
     int num;
     Vec3f translation;
     Vec3f scale;
     Vec4f rotation;
};

struct Animation {
     int frame_num;
     vector<Frame> frames;
};
Animation parse(ifstream &infile);
