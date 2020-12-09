#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <eigen3/Eigen/Dense>
#include <unordered_map>
#include "objectcopy.h"
#include "matrices.h"

using Eigen::MatrixXd;
using namespace std;

/** apply transformation to object */
Object transform_obj(Object obj) {
    MatrixXd m = get_product(obj.transmatrices);
    MatrixXd inv_m = m.inverse();
    for (Vertex *v : obj.vertices) {
        if (v == NULL) continue;
        MatrixXd vec(4, 1);
        vec << v->x, v->y, v->z, 1;
        MatrixXd t_vec = inv_m * vec;
        float w = t_vec(3, 0);
        v->x = t_vec(0, 0)/w;
        v->y = t_vec(1, 0)/w;
        v->z = t_vec(2, 0)/w;
    }
    return obj;
}

int main(int argc, const char* argv[]) {

    /*read file*/

    unordered_map<string, Object> labels;
    vector<ObjectCopy> transformed_labels;
    unordered_map<string, int> num_copy;
    
    ifstream infile(argv[1]);
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
        MatrixXd m;
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
        //count copies of current object
        if (num_copy.count(label) == 0) 
             num_copy[label] = 1;
        else num_copy[label]++;
        obj = transform_obj(obj);
        //store number of copies into the new label; store new label and transformed object in ObjectCopy data structure
        string transformed_label = label + "_copy" + to_string(num_copy[label]);
        ObjectCopy transformed_object;
        transformed_object.label = transformed_label;
        transformed_object.object = obj;
        transformed_labels.push_back(transformed_object);
        }
    /*output*/
    for (ObjectCopy copy: transformed_labels) {
        string label = copy.label;
        Object obj = copy.object;
        cout << label << endl;
        for (int i = 1; i < obj.vertices.size(); i++) { //skip null
            Vertex *v = obj.vertices[i];
            cout << v->x << " " << v->y << " " << v->z << endl;
        }
        cout << "\n" << endl;
    }
}
