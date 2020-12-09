#ifndef PARSER_H
#define PARSER_H

#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include "structures.h"
#include <eigen3/Eigen/Dense>
#include "matrices.h"

using namespace std;
using namespace Eigen;
const string DATA_DIR = "data/";

/**parse the object **/

Object parse_object(string filename) {
        string filename_ = DATA_DIR;
	filename_.append(filename);
        vector<Triple> vertex_buffer;
        vector<Triple> normal_buffer;
        vector<Triple> vertex_sets;
        vector<Triple> normal_sets;
        vertex_sets.push_back(Triple()); // 1-index vertices
        normal_sets.push_back(Triple()); // 1-index normals
        ifstream infile(filename_);
        string t;
        while (infile >> t ) {
            if (t.compare("v") == 0) { //storing a vertex in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Triple v(x, y, z);
                vertex_sets.push_back(v); 
            }
            else if (t.compare("vn") == 0) {//storing a surface normal in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Triple vn(x, y, z);
                normal_sets.push_back(vn);
            } else {
            	 string a, b, c;
            	 infile >> a >> b >> c;
            	 string v1s = a.substr(0, a.find("//"));
            	 string vn1s = a.substr(a.find("//") + 2);
            	 int v1 = stoi(v1s);
            	 int vn1 = stoi(vn1s);
            	 string v2s = b.substr(0, b.find("//"));
            	 string vn2s = b.substr(b.find("//") + 2);
            	 int v2 = stoi(v2s);
            	 int vn2 = stoi(vn2s);
            	 string v3s = c.substr(0, c.find("//"));
            	 string vn3s = c.substr(c.find("//") + 2);
            	 int v3 = stoi(v3s);
            	 int vn3 = stoi(vn3s);
            	 //create the large arrays
            	 vertex_buffer.push_back(vertex_sets[v1]);
            	 vertex_buffer.push_back(vertex_sets[v2]);
            	 vertex_buffer.push_back(vertex_sets[v3]);
            	 normal_buffer.push_back(normal_sets[vn1]);
            	 normal_buffer.push_back(normal_sets[vn2]);
            	 normal_buffer.push_back(normal_sets[vn3]); 
            }
        }
        Object o(vertex_buffer, normal_buffer);
        return o;
}

vector<Object> parse_objects(ifstream &infile) {
    unordered_map<string, Object> labels;
    vector<Object> object_copies;
    string line;
    getline(infile, line);
    
    /* the labels and corresponding filenames come up before an empty line
       so fill in the map before encountering empty line */
    while (!line.empty()) {
        istringstream iss(line);
        string label, file;
        iss >> label >> file;
        labels[label] = parse_object(file);
        getline(infile, line);
    }
    
    /* read the rest of the file, i.e. labels of objects and transformations */
    while (getline(infile, line)) {
        //first line is a label for an object
        string label = line;
        Object obj = labels[label];
        getline(infile, line);
        Matrix4f m;
        //next few nonempty lines are transformation matrices for given object or material parameters. Also material parameters.
        while (!line.empty()) {
            string t;
            istringstream iss(line);
            iss >> t;
            if (t.compare("t") == 0 || t.compare("s") == 0) {
                float x, y, z;
                iss >> x >> y >> z;
                Transforms transform(t, x, y, z);
                obj.transform_sets.push_back(transform);
            }
            else if (t.compare("r") == 0) {
                float x, y, z, angle;
                iss >> x >> y >> z >> angle;
                Transforms transform(t, x, y, z, angle);
                obj.transform_sets.push_back(transform);
            }
            else if (t.compare("ambient") == 0) {
                float r, g, b;
                iss >> r >> g >> b;
                obj.SetAmbient(r, g, b);
	     }
	    else if (t.compare("diffuse") == 0) {
	        float r, g, b;
                iss >> r >> g >> b;
                obj.SetDiffuse(r, g, b);
	    }
	    else if (t.compare("specular") == 0) {
	        float r, g, b;
                iss >> r >> g >> b;
                obj.SetSpecular(r, g, b);
	    }
	    else if (t.compare("shininess") == 0) {
	        float shine;
                iss >> shine;
                obj.shininess = shine;
	    }
            if (infile.eof())
                break;
            getline(infile, line);
        }
    object_copies.push_back(obj);
    }
    return object_copies;
}

Scene parse_camera(ifstream &infile) {
	Scene scene;
	string line, t;
	float x, y, z, angle, p, r, g, b, atten;
	char van;
	//read camera parameters
	while (getline(infile, line)) {
	if (line.compare("objects:") == 0) {
            scene.objects = parse_objects(infile); //call parse_objects when reaching objects
            break;
        } else {
		istringstream iss(line);
		iss >> t;
		if (t.compare("camera:") == 0) {
			continue;
		}
		else if (t.compare("position") == 0) {
			iss >> x >> y >> z;
			Triple position_camera(x, y, z);
			scene.position_camera = position_camera;
		}
		else if (t.compare("orientation") == 0) {
			iss >> x >> y >> z >> angle;
			Triple orientation_camera(x, y, z);
			scene.orientation_camera = orientation_camera;
			scene.orientation_angle_cam = angle;
		}
		else if (t.compare("near") == 0) {
			iss >> p;
			scene.near = p;
		}
		else if (t.compare("far") == 0) {
			iss >> p;
			scene.far = p;
		}
		else if (t.compare("left") == 0) {
			iss >> p;
			scene.left = p;
		}
		else if (t.compare("right") == 0) {
			iss >> p;
			scene.right = p;
		}
		else if (t.compare("top") == 0) {
			iss >> p;
			scene.top = p;
		}
		else if (t.compare("bottom") == 0) {
			iss >> p;
			scene.bottom = p;
		}
		else if (t.compare("light") == 0) {
			iss >> x >> y >> z >> van >> r >> g >> b >> van >> atten;
			Point_Light l(x, y, z, 1, r, g, b, atten);
			scene.lights.push_back(l);
		}
	 }
	}
	return scene;
}

Vector3f get_cam_position(Scene s) {
   Matrix4f trans = get_trans(s.position_camera.x, s.position_camera.y, s.position_camera.z);
   Matrix4f rot = get_rot(s.orientation_camera.x, s.orientation_camera.y, s.orientation_camera.z, s.orientation_angle_cam);
   Matrix4f m = rot * trans;
   Vector4f coords;
   coords << 0, 0, 0, 1;
   coords = m * coords;
   float w = coords(3);
   Vector3f res;
   res << coords(0)/w, coords(1)/w, coords(2)/w;
   return res;
}


#endif
