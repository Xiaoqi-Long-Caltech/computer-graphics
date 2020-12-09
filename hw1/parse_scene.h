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
        o.vertices.push_back(null_v); //as instructed
        
        ifstream infile(filename_);
        char t;
        while (infile >> t ) {
            if (t == 'v') { //storing a vertex in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Vertex v{x, y, z};
                o.vertices.push_back(v); 
            }
            else if (t == 'f') {//storing a face in our data structure
                int v1, v2, v3;
                infile >> v1 >> v2 >> v3;
                Face f{v1, v2, v3};
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
        //next few nonempty lines are transformation matrices for given object
        while (!line.empty()) {
            char t;
            istringstream iss(line);
            iss >> t;
            if (t == 't') {
                float x, y, z;
                iss >> x >> y >> z;
                m = get_trans(x, y, z);
            }
            else if (t == 's') {
                float x, y, z;
                iss >> x >> y >> z;
                m = get_scale(x, y, z);
            }
            else if (t == 'r') {
                float x, y, z, angle;
                iss >> x >> y >> z >> angle;
                m = get_rot(x, y, z, angle);
            }
            obj.transmatrices.push_back(m);
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
	float x, y, z, angle, p;
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
	}
	}
	return scene;
}

#endif
