#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include "parse_scene.h"

using Eigen::Matrix4f;
using namespace std;

/**Part 2: Create the transformation matrix for converting between 
world coordinates and camera coordinates. **/

/* Transforms from world space to camera space */

Matrix4f inv_camera_transformation_matrix(Vertex position, Orientation orientation) {
      Matrix4f trans = get_trans(position.x, position.y, position.z);
      Matrix4f rot = get_rot(orientation.x, orientation.y, orientation.z, orientation.angle);
      Matrix4f C = trans * rot;
      Matrix4f C_inv = C.inverse();
      return C_inv;
}

Matrix4f perspective_projection_matrix(Scene scene) {
      float n = scene.near;
      float f = scene.far;
      float r = scene.right;
      float l = scene.left;
      float b = scene.bottom;
      float t = scene.top;
      Matrix4f m;
      m <<
        (2 * n) / (r - l), 0, (r + l) / (r - l), 0,
        0, (2 * n) / (t - b), (t + b) / (t - b), 0,
        0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n),
        0, 0, -1, 0;
      return m;
}

/**Part 3**/
void output_object_vertices(Object o) {
    for (Vertex v : o.vertices) {
            cout << "object:" << v.x << " " << v.y << " " << v.z << " " << v.screen_x << " " << v.screen_y << "  "  << endl;
    }
}

/** apply transformation to object */
void transform_obj(Object& obj, Matrix4f m) {
    vector<Vertex> new_vertices;
    for (Vertex v : obj.vertices) {
        if (v.x == 0 && v.y == 0 && v.z == 0) continue;
        float x, y, z;
        Vector4f vec;
        vec << v.x, v.y, v.z, 1;
        Vector4f t_vec = m * vec;
        float w = t_vec(3, 0);
        x = t_vec(0, 0)/w;
        y = t_vec(1, 0)/w;
        z = t_vec(2, 0)/w;
        Vertex new_vertex{x, y, z};
        new_vertices.push_back(new_vertex);
    }
    obj.SetVertices(new_vertices);
}

/** applying geometric transformation */
void geometric_trans(Object& obj) {
     Matrix4f m = get_product(obj.transmatrices);
     transform_obj(obj, m);
}

/** transforming points into NDC */
void NDC_trans(Object& obj, Scene& scene){
     Matrix4f m = perspective_projection_matrix(scene);
     transform_obj(obj, m);
}

/** transforming from world view to camera view **/
void camera_trans(Object& obj, Scene& scene){
     Matrix4f m = inv_camera_transformation_matrix(scene.position, scene.orientation);
     transform_obj(obj, m);
}


/**Part 4**/

void output_object_vertice(Object o) {
    for (Vertex v : o.vertices) {
            cout << v.x << " " << v.y << " " << v.z << " " << v.screen_x << " " << v.screen_y << "  "  << endl;
    }
}

/** Map from points in NDC to screen coordinates in a yres by xres pixel grid */
void NDC_to_screen(Scene& scene, int xres, int yres) {
     vector<Object> new_objects;
     for (Object o: scene.objects){
        vector<Vertex> new_vertices;
        for (Vertex v : o.vertices) {
           if (v.x == 0 & v.y == 0 & v.z == 0) continue;
           //discard points that were mapped to the outside of the perspective cube
           if (abs(v.x) >= 1 || abs(v.y) >= 1)
              continue;
           //make sure everything is positive, and scale with resolution params
           v.SetScreen((v.x + 1) * xres / 2, (v.y + 1) * yres / 2);
           new_vertices.push_back(v);
        }
        o.SetVertices(new_vertices);
        new_objects.push_back(o);
     }
     scene.SetObjects(new_objects);
}

#endif



