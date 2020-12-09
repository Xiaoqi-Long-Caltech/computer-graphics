#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "Vertex.h"
#include "Face.h"
#include <fstream>
#include <cassert>

using namespace std;

/** Object including a vector of vertices and a vector of faces  **/
struct Object {
    vector<Vertex *> vertices;
    vector<Face *> faces;
    Object(vector<Vertex *> vertices_, vector<Face *> faces_)
	: vertices(vertices_), faces(faces_) {}
};

int main(int argc, const char *argv[]) {
    
    vector<Object *> objects; //used to store each object file
    
    /**input */
    for (int i = 1; i < argc; i++) { // skip name of the program
       ifstream infile(argv[i]);
    	vector<Vertex *> vertices;
    	vector<Face *> faces;
        vertices.push_back(NULL); //as instructed
        
        char t;
        while (infile >> t ) {
            if (t == 'v') { //storing a vertex in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Vertex *ver = new Vertex(x, y, z);
                vertices.push_back(ver); 
            }
            else if (t == 'f') {//storing a face in our data structure
                int v1, v2, v3;
                infile >> v1 >> v2 >> v3;
                Face *fa = new Face(v1, v2, v3);
                faces.push_back(fa);
            }
        }
        Object *o = new Object(vertices, faces);
        objects.push_back(o); //add to vector of objects
    }

    /**output */
    for (int i = 0; i < (int) objects.size(); i++) {
        cout << "obj_file" << i + 1 << ":" << endl; 
        cout << "\n";
        //print v
        for (Vertex* v: objects[i]->vertices) {
        	if (v != NULL)
            	cout << "v " << v->x << " " << v->y << " " << v->z << endl;
        }
        //print face
        for (Face* f: objects[i]->faces) {
            cout << "f " << f->v1 << " " << f->v2 << " " << f->v3 << endl;
        }
        cout << "\n";
    }

}
