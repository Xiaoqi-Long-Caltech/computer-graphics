#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "parse_scene.h"

using namespace std;

/**Most of the additional implementations of assignment 5 are in this file. Descriptions of functions are in the header file.**/

Object make_copy(Object o) {
    Object res;
    //copy everything from one object to another
    res.vertex_buffer = o.vertex_buffer;
    res.normal_buffer = o.normal_buffer;
    res.transform_sets = o.transform_sets;
    res.faces = o.faces;
    res.normal_sets = o.normal_sets;
    res.vertex_sets = o.vertex_sets;
    res.ambient_reflect[0] = o.ambient_reflect[0];
    res.ambient_reflect[1] = o.ambient_reflect[1];
    res.ambient_reflect[2] = o.ambient_reflect[2];
    res.diffuse_reflect[0] = o.diffuse_reflect[0];
    res.diffuse_reflect[1] = o.diffuse_reflect[1];
    res.diffuse_reflect[2] = o.diffuse_reflect[2];
    res.specular_reflect[0] = o.specular_reflect[0];
    res.specular_reflect[1] = o.specular_reflect[1];
    res.specular_reflect[2] = o.specular_reflect[2];
    res.shininess = o.shininess;
    return res;
}

Object parse_object(string filename, Mesh_Data *mesh_data) {
        vector<Vertex> vertex_buffer;
        vector<Vertex> vertex_sets;
        vector<Vertex *> *mesh_vertices = new vector<Vertex *>();
        mesh_vertices->push_back(NULL);
        vector<Face> faces;
        vector<Face *> *mesh_faces = new vector<Face *>();
        
        vertex_sets.push_back(Vertex()); // 1-index vertices
        
        ifstream infile(filename);
        string t;
        while (infile >> t ) {
            if (t.compare("v") == 0) { //storing a vertex in our data structure
                float x, y, z;
                infile >> x >> y >> z;
                Vertex v(x, y, z);
                vertex_sets.push_back(v); 
                Vertex *mesh_v;
                mesh_v = new Vertex(x, y, z);
                mesh_vertices->push_back(mesh_v); //store into mesh_data
            }
            else if (t.compare("f") == 0) {
            	 int idx1, idx2, idx3;
            	 infile >> idx1 >> idx2 >> idx3;
            	 vertex_buffer.push_back(vertex_sets[idx1]);
            	 vertex_buffer.push_back(vertex_sets[idx2]);
            	 vertex_buffer.push_back(vertex_sets[idx3]);
            	 Face *mesh_f;
            	 mesh_f = new Face(idx1, idx2, idx3);
            	 mesh_faces->push_back(mesh_f);
            }
        }
        Object o;
        o.vertex_sets =  vertex_sets;
        o.vertex_buffer = vertex_buffer;
        o.faces = faces;
        mesh_data->vertices = mesh_vertices;
        mesh_data->faces = mesh_faces;
        
        return o;
}

void parse_objects(ifstream &infile, Mesh_Data *mesh_data, vector<Object> object_copies, vector<Object> smooth_object_copies, float h, Scene& scene) {
    unordered_map<string, Object> labels;
    string line;
    getline(infile, line);
    
    /* the labels and corresponding filenames come up before an empty line
       so fill in the map before encountering empty line */
    while (!line.empty()) {
        istringstream iss(line);
        string label, file;
        iss >> label >> file;
        labels[label] = parse_object(file, mesh_data);
        getline(infile, line);
    }
    /* read the rest of the file, i.e. labels of objects and transformations */
    while (getline(infile, line)) {
        //first line is a label for an object
        string label = line;
        //store parsed data as copies into obj and smooth_obj
        Object obj = make_copy(labels[label]);
        Object smooth_obj = make_copy(labels[label]);
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
                smooth_obj.transform_sets.push_back(transform);
            }
            else if (t.compare("r") == 0) {
                float x, y, z, angle;
                iss >> x >> y >> z >> angle;
                Transforms transform(t, x, y, z, angle);
                obj.transform_sets.push_back(transform);
                smooth_obj.transform_sets.push_back(transform);
            }
            else if (t.compare("ambient") == 0) {
                float r, g, b;
                iss >> r >> g >> b;
                obj.SetAmbient(r, g, b);
                smooth_obj.SetAmbient(r, g, b);
	     }
	    else if (t.compare("diffuse") == 0) {
	        float r, g, b;
                iss >> r >> g >> b;
                obj.SetDiffuse(r, g, b);
                smooth_obj.SetDiffuse(r, g, b);
	    }
	    else if (t.compare("specular") == 0) {
	        float r, g, b;
                iss >> r >> g >> b;
                obj.SetSpecular(r, g, b);
                smooth_obj.SetSpecular(r, g, b);
	    }
	    else if (t.compare("shininess") == 0) {
	        float shine;
                iss >> shine;
                obj.shininess = shine;
                smooth_obj.shininess = shine;
	    }
            if (infile.eof())
                break;
            getline(infile, line);
        }
    object_copies.push_back(obj);
    smooth_object_copies.push_back(smooth_obj);
    }
    scene.objects = object_copies;
    scene.smooth_objects = smooth_object_copies;
}

Vector3f hev2vec(HEV *hev) {
         Vector3f vec(hev->x, hev->y, hev->z);
         return vec;
}

Vec3f compute_normal(HEV *v) {
         Vec3f n;
         n.x = 0;
         n.y = 0;
         n.z = 0;
         
         // halfedge going out from the given vertex
         HE* he = v->out;        
         
         do
         {
            HEV *hev1 = he->vertex;
            HEV *hev2 = he->next->vertex;
            HEV *hev3 = he->next->next->vertex;
            
            Vector3f v1 = hev2vec(hev1);
            Vector3f v2 = hev2vec(hev2);
            Vector3f v3 = hev2vec(hev3);
            
            //face normal = cross product of (v2 - v1) x (v3 - v1)
            Vector3f face_normal = (v2 - v1).cross(v3 - v1);
            float face_area = 0.5 * face_normal.norm();
           
            n.x += face_normal(0) * face_area;
            n.y += face_normal(1) * face_area;
            n.z += face_normal(2) * face_area;
            
            //go to the next halfedge
            he = he->flip->next;
         }
         while(he != v->out);
         
         //normalize n
         float m = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
         n.x /= m;
         n.y /= m;
         n.z /= m;
         v->normal = n;
         return n;
}
    
void get_normals(vector<HEV *> *hevs) {
     for(size_t i = 1; i < hevs->size(); i++) {
          Vec3f n = compute_normal(hevs->at(i));
          //store normals into the halfedge data structure
          hevs->at(i)->normal = n;
     }
}

void populate_norm(vector<Object> *objects, Mesh_Data *mesh_data, vector<HEV *> *hevs) { 
         for (size_t i = 0; i < mesh_data->faces->size(); i++) {
             Face *f = mesh_data->faces->at(i);
             int i1 = f->idx1;
             int i2 = f->idx2;
             int i3 = f->idx3;
             Vec3f n1 = hevs->at(i1)->normal;
             Vec3f n2 = hevs->at(i2)->normal;
             Vec3f n3 = hevs->at(i3)->normal;
             (*objects)[0].normal_buffer.push_back(n1);
             (*objects)[0].normal_buffer.push_back(n2);
             (*objects)[0].normal_buffer.push_back(n3);
         }
}

void populate_vertex(vector<Object> *objects, Mesh_Data *mesh_data, vector<HEV *> *hevs) {  
         for (size_t i = 0; i < mesh_data->faces->size(); i++) {
             Face *f = mesh_data->faces->at(i);
             int i1 = f->idx1;
             int i2 = f->idx2;
             int i3 = f->idx3;
             HEV *hev1 = hevs->at(i1);
             HEV *hev2 = hevs->at(i2);
             HEV *hev3 = hevs->at(i3);
             Vertex v1 = {(float)hev1->x, (float)hev1->y, (float)hev1->z};
             Vertex v2 = {(float)hev2->x, (float)hev2->y, (float)hev2->z};
             Vertex v3 = {(float)hev3->x, (float)hev3->y, (float)hev3->z};
             (*objects)[0].vertex_buffer.push_back(v1);
             (*objects)[0].vertex_buffer.push_back(v2);
             (*objects)[0].vertex_buffer.push_back(v3);
       }
}

float compute_area(HEF *face) {
     HEV *hev1 = face->edge->vertex;
     HEV *hev2 = face->edge->next->vertex;
     HEV *hev3 = face->edge->next->next->vertex;
            
     Vector3f v1 = hev2vec(hev1);
     Vector3f v2 = hev2vec(hev2);
     Vector3f v3 = hev2vec(hev3);
            
     //face normal = cross product of (v2 - v1) x (v3 - v1)
     Vector3f face_normal = (v2 - v1).cross(v3 - v1);
     float face_area = 0.5 * face_normal.norm();
     return face_area;
}

float compute_area_sum(HEV *v) {
     float area_sum = 0;
     HE* he = v->out;
     do
     {
        area_sum += compute_area(he->face);
        he = he->flip->next;
     }
     while(he != v->out);
     return area_sum;
}
    

Vector3f u_to_v(HEV *u, HEV *v) {
     Vector3f vec(v->x - u->x, v->y - u->y, v->z - u->z);
     return vec;
}

float compute_cot(HE *he) {
    HEV *hev1 = he->vertex;
    HEV *hev2 = he->next->vertex;
    HEV *alpha = he->next->next->vertex;
    HEV *beta = he->flip->next->next->vertex;
    Vector3f v1_a = u_to_v(hev1, alpha);
    Vector3f v2_a = u_to_v(hev2, alpha);
    Vector3f v1_b = u_to_v(hev1, beta);
    Vector3f v2_b = u_to_v(hev2, beta);
    
    // cot a = cos a / sin a
    float cos_a = (v1_a.dot(v2_a)) / (v1_a.norm() * v2_a.norm());
    float cos_b = (v1_b.dot(v2_b)) / (v1_b.norm() * v2_b.norm());
    float sin_a = (v1_a.cross(v2_a)).norm() / (v1_a.norm() * v2_a.norm());
    float sin_b = (v1_b.cross(v2_b)).norm() / (v1_b.norm() * v2_b.norm());
    // what if sina + sinb = 0? they won't be since a and b are angles in triangles.
    return cos_a / sin_a + cos_b / sin_b;
}
    
void index_vertices(vector<HEV *> *vertices ) {
     for( int i = 1; i < vertices->size(); ++i ) // start at 1 because obj files are 1-indexed
     vertices->at(i)->index = i; // assign each vertex an index
}
    
SparseMatrix<float> build_F_operator(vector<HEV *> *vertices, float h){
       index_vertices(vertices); // assign each vertex an index
       // recall that due to 1-indexing of obj files, index 0 of our list doesnt actually contain a vertex
       int num_vertices = vertices->size() - 1;

       // initialize a sparse matrix to represent our f operator
       // f is not F; f is just the h\Delta as shown in the notes. We will calculate F = I - hf
       SparseMatrix<float> f(num_vertices, num_vertices);

       // reserve room for 7 non-zeros per row of f
       f.reserve(VectorXi::Constant(num_vertices, 7));
       
       for (size_t i = 1; i < vertices->size(); i++) {
           float area = compute_area_sum(vertices->at(i));
           HE *he = vertices->at(i)->out;
           int c = 0;
           if (area > EPSILON) { //if incident area sum is large enough
              do
              {
                int col = he->next->vertex->index;
                float cot = compute_cot(he);
                f.insert(i - 1, col - 1) = 1.0 / (2 * area) * cot;
                c += 1.0 / (2 * area) * cot;
                he = he->flip->next;
              }
              while(he != vertices->at(i)->out);
              // fill in the diagonals
              f.insert(i - 1, i - 1) = -c;
           }
       }
       SparseMatrix<float> id(num_vertices, num_vertices);
       id.setIdentity();
       SparseMatrix<float> F(num_vertices, num_vertices);
       //as promised
       F = id - (h * f);
       F.makeCompressed();    
       return F;
}  

void solve_mesh(vector<HEV *> *hevs, float h, vector<Vertex *> *mesh_vertices) {
     int num_vertices = hevs->size() - 1;
     SparseMatrix<float> F = build_F_operator(hevs, h);
     SparseLU<SparseMatrix<float>, COLAMDOrdering<int>> solver;
     solver.analyzePattern(F);
     solver.factorize(F);
     //fill in x_0, y_0, z_0
     VectorXf x0(num_vertices);
     VectorXf y0(num_vertices);
     VectorXf z0(num_vertices);
     for (int i = 1; i < hevs->size(); i++) {
          x0(i - 1) = hevs->at(i)->x;
          y0(i - 1) = hevs->at(i)->y;
          z0(i - 1) = hevs->at(i)->z;
     }
     VectorXf xh(num_vertices);
     VectorXf yh(num_vertices);
     VectorXf zh(num_vertices);
     //solve for smoothened x_h, y_h, z_h
     xh = solver.solve(x0);
     yh = solver.solve(y0);
     zh = solver.solve(z0);

     //fill in the new vertices
     for (size_t i = 1; i < hevs->size(); i++) {
         float x_, y_, z_;
         x_ = xh(i - 1);
         y_ = yh(i - 1);
         z_ = zh(i - 1);
         Vertex *v = new Vertex(x_, y_, z_);
         mesh_vertices->push_back(v);        
     }
}

vector<Object> update(vector<HEV *> *hevs, float h, Mesh_Data *mesh_data, vector<Object> smooth_objects) {
    Mesh_Data *smooth_mesh_data = new Mesh_Data;
    vector<Vertex *> *smooth_mesh_vertices = new vector<Vertex *>();
    smooth_mesh_vertices->push_back(NULL);
    //update the vertices in the smoothened mesh
    solve_mesh(hevs, h, smooth_mesh_vertices);
    //faces are inherited from the original mesh
    vector<Face *> *smooth_mesh_faces = mesh_data->faces;
    smooth_mesh_data->vertices = smooth_mesh_vertices;
    smooth_mesh_data->faces = smooth_mesh_faces;
    vector<HEV *> *hevs_smooth = new vector<HEV *>();
    vector<HEF *> *hefs_smooth = new vector<HEF *>();
    //build halfedge data again
    build_HE(smooth_mesh_data, hevs_smooth, hefs_smooth);
    
    smooth_objects[0].vertex_buffer.clear();
    smooth_objects[0].normal_buffer.clear();
    //populate the vertices into objects
    populate_vertex(&smooth_objects, smooth_mesh_data, hevs_smooth);
    //compute normals and populate them
    get_normals(hevs_smooth);
    populate_norm(&smooth_objects, smooth_mesh_data, hevs_smooth);
    return smooth_objects;
}
