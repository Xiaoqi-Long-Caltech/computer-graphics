#include <vector>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "Vertex.h"
#include "Face.h"
#include <fstream>
#include <cassert>

using Eigen::MatrixXd;
using namespace std;

/**Object struct including a vector of pointers to vertices, a vector of pointers to faces, a vector of pointers to transformation matrices  **/
/** added assignment operator for Part 3 */

struct Object {
    vector<Vertex *> vertices;
    vector<Face *> faces;
    vector<MatrixXd> transmatrices;
    Object(const Object &o2) {
         vector<Vertex *> new_vertices;
         vector<Face *> new_faces;
         for (Vertex *v : o2.vertices) {
            Vertex *new_v = NULL;
            if (v != NULL)
               new_v = new Vertex(*v);
            new_vertices.push_back(new_v);
         }
         for (Face *f : o2.faces){
            Face *new_f = new Face(*f);
            new_faces.push_back(new_f);
         }
         vertices = new_vertices;
         faces = new_faces;
         transmatrices = o2.transmatrices;
    }
    Object() {};
};

/** ObjectCopy struct used to store desired printed labels and the objects */

struct ObjectCopy {
     string label;
     Object object;
};

/** Main function from Part1 integrated into a function. Takes in a filename and return a pointer of the above defined object*/
Object parse_object(string filename) {
        Object o;
        o.vertices.push_back(NULL); //as instructed
        
        ifstream infile(filename);
        char t;
        while (infile >> t ) {
            if (t == 'v') { //storing a vertex in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Vertex *ver = new Vertex(x, y, z);
                o.vertices.push_back(ver); 
            }
            else if (t == 'f') {//storing a face in our data structure
                int v1, v2, v3;
                infile >> v1 >> v2 >> v3;
                Face *fa = new Face(v1, v2, v3);
                o.faces.push_back(fa);
            }
        }
        return o;
}
