#ifndef PARSER_H
#define PARSER_H

#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include "structures.h"
#include <unordered_map>
#include "matrices.h"

using namespace std;
using Eigen::Matrix4f;
const string DATA_DIR = "data/";

/**Part 1: Parse the scene description file and put the data in a 
Scene data structure **/

/**parse the object **/

Object parse_object(string filename) {
       string filename_ = DATA_DIR;
	filename_.append(filename);
        Object o;
        Vertex null_v{0, 0, 0};
        Normal null_m{0, 0, 0};
        o.vertices.push_back(null_v); //as instructed
        o.normals.push_back(null_m);
        
        ifstream infile(filename_);
        string t;
        while (infile >> t ) {
            if (t.compare("v") == 0) { //storing a vertex in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Vertex v{x, y, z};
                o.vertices.push_back(v); 
            }
            else if (t.compare("vn") == 0) {//storing a surface normal in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Normal vn{x, y, z};
                o.normals.push_back(vn);
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
            	 Face f(v1, v2, v3, vn1, vn2, vn3);
            	 o.faces.push_back(f);
            }
        }
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
            if (t.compare("t") == 0) {
                float x, y, z;
                iss >> x >> y >> z;
                m = get_trans(x, y, z);
                obj.transmatrices.push_back(m);
            }
            else if (t.compare("s") == 0) {
                float x, y, z;
                iss >> x >> y >> z;
                m = get_scale(x, y, z);
                obj.transmatrices.push_back(m);
                obj.normal_trans.push_back(m);
            }
            else if (t.compare("r") == 0) {
                float x, y, z, angle;
                iss >> x >> y >> z >> angle;
                m = get_rot(x, y, z, angle);
                obj.transmatrices.push_back(m);
                obj.normal_trans.push_back(m);
            }
            else if (t.compare("ambient") == 0) {
                float r, g, b;
                iss >> r >> g >> b;
                Color amb(r, g, b);
                obj.material.ambient = amb;
	     }
	    else if (t.compare("diffuse") == 0) {
	        float r, g, b;
                iss >> r >> g >> b;
                Color diff(r, g, b);
                obj.material.diffuse = diff;
	    }
	    else if (t.compare("specular") == 0) {
	        float r, g, b;
                iss >> r >> g >> b;
                Color spec(r, g, b);
                obj.material.specular = spec;
	    }
	    else if (t.compare("shininess") == 0) {
	        float shine;
                iss >> shine;
                obj.material.shininess = shine;
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
			Vertex position(x, y, z);
			scene.position = position;
		}
		else if (t.compare("orientation") == 0) {
			iss >> x >> y >> z >> angle;
			Orientation orientation(x, y, z, angle);
			scene.orientation = orientation;
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
			Light l(x, y, z, r, g, b, atten);
			scene.lights.push_back(l);
		}
	 }
	}
	return scene;
}

#endif
