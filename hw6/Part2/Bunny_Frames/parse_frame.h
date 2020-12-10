#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

struct Vec3f
{
	float x, y, z;
	Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vec3f() {}
	// need to_string method to output into .obj files
	string v_to_string() {
          ostringstream res;
          res << setprecision(6) << fixed;
          res << x << " " << y << " " << z;
          return res.str();
        }
        string f_to_string() {
          ostringstream res;
          res << x << " " << y << " " << z;
          return res.str();
        }
};

struct KeyFrame {
     int num;
     vector<Vec3f> vertices;
     vector<Vec3f> faces;
     string to_string() {
        ostringstream res;
        for (Vec3f v : vertices)
           res << "v " << v.v_to_string() << endl;
        for (Vec3f f : faces)
           res << "f " << f.f_to_string() << endl;
        return res.str();
     }
};

struct Animation {
     int frame_num;
     vector<KeyFrame> frames;
};

KeyFrame parse_keyframe(ifstream &infile, int num);
Animation parse_all();
