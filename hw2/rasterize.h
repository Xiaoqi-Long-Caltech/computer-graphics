#ifndef RASTERIZE_H
#define RASTERIZE_H

#include <math.h>
#include <stdio.h>
#include "structures.h"
#include "transform.h"
#include <eigen3/Eigen/Dense>
#include <iostream>
using namespace std;
using namespace Eigen;

/* Part 4: Implement the algorithm for the lighting model*/
Color Lighting(Vertex v, Normal n, Material m, Scene& s) {
    Vector3f vertex_pos = v.GetCoords();
    Vector3f norm_vec = n.GetNormal().normalized(); //normalize normal vectors
    Vector3f amb = m.ambient.GetColor();
    Vector3f diff = m.diffuse.GetColor();
    Vector3f spec = m.specular.GetColor();
    float shine = m.shininess;

    Vector3f diffuse_sum(3, 1);
    diffuse_sum << 0, 0, 0;
    Vector3f specular_sum(3, 1);
    specular_sum << 0, 0, 0;
    
    Vector3f cam_pos = s.position.GetCoords();
    Vector3f direction = cam_pos - vertex_pos;
    direction.normalize();
    
    for (Light l : s.lights) {
        Vector3f l_position = l.position.GetCoords();
        Vector3f l_color = l.c.GetColor();
        Vector3f l_dist = l_position - vertex_pos;
        Vector3f l_direction = l_dist.normalized(); 
        //attenuation
        float l_distance = l_dist.norm();
        float atten_factor = 1.0/ (1.0 + l.k * l_distance * l_distance);
        l_color *= atten_factor;
        //diffuse
        float prod_diffuse = norm_vec.dot(l_direction);
        Vector3f l_diffuse = l_color * max((float)0, prod_diffuse);
        diffuse_sum += l_diffuse;
        //specular
        float prod_specular = norm_vec.dot((direction + l_direction).normalized());
        Vector3f l_specular = l_color * pow(max((float)0, prod_specular), shine);
        specular_sum += l_specular;
    }

    Vector3f ones(1.0, 1.0, 1.0);
    Vector3f color = amb + diffuse_sum.cwiseProduct(diff) + specular_sum.cwiseProduct(spec);
    Vector3f min_c = ones.cwiseMin(color);
    Color res_color(min_c(0, 0), min_c(1, 0), min_c(2, 0));
    return res_color;
}


/*Part 5: Implement the algorithm for rasterizing colored triangles with interpolation
 * via barycentric coordinates, backface culling, and depth buffering. */
 
float area(int x, int y, int x_i, int y_i, int x_j, int y_j) {
     float res = (y_i - y_j) * x + (x_j - x_i) * y + x_i * y_j - x_j * y_i;
     return res;
}
 
float compute_alpha(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c, int x, int y) {
     return area(x, y, x_b, y_b, x_c, y_c) / area(x_a, y_a, x_b, y_b, x_c, y_c);
}

float compute_beta(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c, int x, int y) {
     return area(x, y, x_a, y_a, x_c, y_c) / area(x_b, y_b, x_a, y_a, x_c, y_c);
}

float compute_gamma(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c, int x, int y) {
     return area(x, y, x_a, y_a, x_b, y_b) / area(x_c, y_c, x_a, y_a, x_b, y_b);
}

void Raster_triangle(Vertex a, Vertex b, Vertex c, Color& a_c, Color& b_c, Color& c_c, MatrixColor& grid, Scene& s) {
      int xres = grid.cols();
      int yres = grid.rows();
      
      Vector3f NDC_a = a.GetCoords();
      Vector3f NDC_b = b.GetCoords();
      Vector3f NDC_c = c.GetCoords();
      
      //Backface culling
      Vector3f cross = (NDC_c - NDC_b).cross(NDC_a - NDC_b);
      if(cross(2, 0) < 0){
         return;
      }
      
      Vertex_NDC_to_screen(a, xres, yres);
      Vertex_NDC_to_screen(b, xres, yres);
      Vertex_NDC_to_screen(c, xres, yres);
      float x_a = a.screen_x;
      float y_a = a.screen_y;
      float x_b = b.screen_x;
      float y_b = b.screen_y;
      float x_c = c.screen_x;
      float y_c = c.screen_y;

      int x_min = min({x_a, x_b, x_c});
      int x_max = max({x_a, x_b, x_c});
      int y_min = min({y_a, y_b, y_c});
      int y_max = max({y_a, y_b, y_c});
      
      for (int x = x_min; x <= x_max; x++) {
          for (int y = y_min; y <= y_max; y++) {
              float alpha = compute_alpha(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
              float beta = compute_beta(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
              float gamma = compute_gamma(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
              if ((alpha >= 0 && alpha <= 1) && (beta >= 0 && beta <= 1)
                    && (gamma >= 0 && gamma <= 1)) {
                    Vector3f NDC = (alpha * NDC_a) + (beta * NDC_b) + (gamma * NDC_c);
                    // check if in cube
                    bool NDC_cube = abs(NDC(0, 0)) <= 1 && abs(NDC(1, 0)) <= 1
                         && abs(NDC(2, 0)) <= 1;
                    if(NDC_cube && NDC(2, 0) <= s.depth_buffer(y, x)) {
                         //depth buffering
                         s.SetDepth(y, x, NDC(2,0));
                         float R = alpha * a_c.r + beta * b_c.r + gamma * c_c.r;
                         float G = alpha * a_c.g + beta * b_c.g + gamma * c_c.g;
                         float B = alpha * a_c.b + beta * b_c.b + gamma * c_c.b;
                         Color c(R, G, B);
                         grid(yres - 1 - y, x) = c;
                    }
              }
          }
      }
}

/* Part 6: Implement the full Gouraud shading algorithm */
void Gouraud_shading(Face& f, Object& obj, Scene& s, MatrixColor& grid) {
     //zero index here because got rid of place holder after transforming vertices
     Vertex a = obj.vertices[f.v1 - 1];
     Vertex b = obj.vertices[f.v2 - 1];
     Vertex c = obj.vertices[f.v3 - 1];
     Normal na = obj.normals[f.vn1 - 1];
     Normal nb = obj.normals[f.vn2 - 1];
     Normal nc = obj.normals[f.vn3 - 1];
     
     Color c1 = Lighting(a, na, obj.material, s);
     Color c2 = Lighting(b, nb, obj.material, s);
     Color c3 = Lighting(c, nc, obj.material, s);
     
     f.SetC1(c1);
     f.SetC2(c2);
     f.SetC3(c3);
     
     Vertex NDC_a = world_to_NDC(a, s);
     Vertex NDC_b = world_to_NDC(b, s);
     Vertex NDC_c = world_to_NDC(c, s);

     Raster_triangle(NDC_a, NDC_b, NDC_c, f.c1, f.c2, f.c3, grid, s);
}

/*Part 7: Implement the full Phong shading algorithm */
void Phong_shading(Face f, Object& obj, Scene& s, MatrixColor& grid) {
     int xres = grid.cols();
     int yres = grid.rows();
     //zero index here because got rid of place holder after transforming vertices
     Vertex a = obj.vertices[f.v1 - 1];
     Vertex b = obj.vertices[f.v2 - 1];
     Vertex c = obj.vertices[f.v3 - 1];
     Normal n_a = obj.normals[f.vn1 - 1];
     Normal n_b = obj.normals[f.vn2 - 1];
     Normal n_c = obj.normals[f.vn3 - 1];
     Vector3f na = n_a.GetNormal();
     Vector3f nb = n_b.GetNormal();
     Vector3f nc = n_c.GetNormal();
     
     Vertex NDC_a_v = world_to_NDC(a, s);
     Vertex NDC_b_v = world_to_NDC(b, s);
     Vertex NDC_c_v = world_to_NDC(c, s);
 
     Vector3f NDC_a = NDC_a_v.GetCoords();
     Vector3f NDC_b = NDC_b_v.GetCoords();
     Vector3f NDC_c = NDC_c_v.GetCoords();
     Vector3f v_a = NDC_a_v.GetWorld();
     Vector3f v_b = NDC_b_v.GetWorld();
     Vector3f v_c = NDC_c_v.GetWorld();
     
     Vector3f cross = (NDC_c - NDC_b).cross(NDC_a - NDC_b);
     if (cross(2, 0) < 0)
        return;
     
     Vertex_NDC_to_screen(NDC_a_v, xres, yres);
     Vertex_NDC_to_screen(NDC_b_v, xres, yres);
     Vertex_NDC_to_screen(NDC_c_v, xres, yres);
     float x_a = NDC_a_v.screen_x;
     float y_a = NDC_a_v.screen_y;
     float x_b = NDC_b_v.screen_x;
     float y_b = NDC_b_v.screen_y;
     float x_c = NDC_c_v.screen_x;
     float y_c = NDC_c_v.screen_y;
      
     int x_min = min({x_a, x_b, x_c});
     int x_max = max({x_a, x_b, x_c});
     int y_min = min({y_a, y_b, y_c});
     int y_max = max({y_a, y_b, y_c});
     
     for (int x = x_min; x <= x_max; x++) {
          for (int y = y_min; y <= y_max; y++) {
              float alpha = compute_alpha(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
              float beta = compute_beta(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
              float gamma = compute_gamma(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
              if ((alpha >= 0 && alpha <= 1) && (beta >= 0 && beta <= 1)
                    && (gamma >= 0 && gamma <= 1)) {
                    Vector3f NDC = (alpha * NDC_a) + (beta * NDC_b) + (gamma * NDC_c);
                    bool NDC_cube = abs(NDC(0, 0)) <= 1 && abs(NDC(1, 0)) <= 1
                         && abs(NDC(2, 0)) <= 1;
                    if(NDC_cube && NDC(2, 0) <= s.depth_buffer(y, x)) {
                         s.depth_buffer(y, x) = NDC(2, 0);
                         Vector3f n_ = (alpha * na) + (beta * nb) + (gamma * nc);
                         Vector3f v_ = (alpha * v_a) + (beta * v_b) + (gamma * v_c);
                         Vertex v{v_(0, 0), v_(1, 0), v_(2, 0)};
                         Normal n{n_(0, 0), n_(1, 0), n_(2, 0)};
                         Color c = Lighting(v, n, obj.material, s);
                         grid(yres - 1 - y, x) = c;
                    }
              }
          }
      }
     
}

/*Part 8: Output to PPM file */
void print_ppm(int xres, int yres, MatrixColor& grid) {
    cout << "P3" << endl
       << xres << " " << yres << endl
       << "255" << endl;

    for (int j = 0; j < yres; j++) {
      for (int i = 0; i < xres; i++) {
         Color c = grid(j, i);
         int red = (int)(c.r * 255);
         int green = (int)(c.g * 255);
         int blue = (int)(c.b * 255);
         cout << red << " " << green << " " << blue << endl;
      }
    }
}
#endif

