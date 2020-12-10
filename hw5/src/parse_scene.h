#ifndef PARSER_H
#define PARSER_H

#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "structures.h"

/*makes a copy of object o; used to store the same data in scene.objects and scene.smooth_objects when initially parsing. */
Object make_copy(Object o);

/*fills in the normals of a HEV * vector; used in next function to populate the normals of an object */
void get_normals(vector<HEV *>*);

/*populate the normal_buffer of an object, given mesh data. */
void populate_norm(vector<Object>*, Mesh_Data *, vector<HEV *> *);

/*perform implicit fairing on a smooth_object. In this function, we solve for the new mesh data, and rebuild the halfedge data structure. Using the new halfedge data, we repopulate the vertex_buffer and normal_buffer of the smooth_object.*/
vector<Object> update(vector<HEV *>*, float, Mesh_Data *, vector<Object>);

/*fill in the object data structure from the file, as well as the global variable mesh_data.*/
Object parse_object(string filename, Mesh_Data *mesh_data);

/*fill in the objects and smooth_objects, along with their transformation matrices and other attributes into the global scene structure. Initially, they have the same data. */
void parse_objects(ifstream &, Mesh_Data *, vector<Object>, vector<Object>, float, Scene&);

/*transform a HEV data structure to Vector3f to perform linear algebra more easily using Eigen*/
Vector3f hev2vec(HEV *);

/*compute the normal of a vertex*/
Vec3f compute_normal(HEV *);

/*populate the new vertices after smoothing. */
void populate_vertex(vector<Object> *, Mesh_Data *, vector<HEV *> *);

/*compute the area of a face*/
float compute_area(HEF *);

/*compute the sum of the incident faces of a vertex. */
float compute_area_sum(HEV *);

/*return the vector from u to v*/
Vector3f u_to_v(HEV *, HEV *);

/*compute the cotangent sum as instructed for a halfedge */
float compute_cot(HE *);

/*index vertices a instructed*/
void index_vertices(vector<HEV *> *);

/*build the F operator using the computed area and cotangent sum */
Eigen::SparseMatrix<float> build_F_operator(vector<HEV *> *, float);

/* solve for the updated xh, yh, and zh and push back the new vertices into the mesh_data variable. */
void solve_mesh(vector<HEV *> *, float, vector<Vertex *> *);

#endif
