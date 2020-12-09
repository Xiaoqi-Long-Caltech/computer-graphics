#ifndef RASTERIZE_H
#define RASTERIZE_H

#include <stdio.h>
#include "structures.h"
#include <eigen3/Eigen/Dense>
#include <iostream>
using namespace std;

/** Part 5 */

/**The generalized Bresenham's line algorithm. The Bresenham's line algorithm in the first 
octant requires that the slope is positive and absolute value no greater than 1. To deal 
with slope that is negative, we switch from incrementing x or y to decrementing. To deal 
with slope with absolute value greater than 1, we iterate on the y values instead of the
x values. **/

/** pixels is a vector of integers to store in values of 0 or 1. For values of 1, the
corresponding pixel is colored **/
void fill(int x, int y, int xres, int yres, vector<int>& pixels) {
       pixels[(yres - y - 1) * xres + x] = 1;
     }

void draw_line(Vertex v1, Vertex v2, vector<int>& pixels, int xres, int yres) {
       //clear up mistakes
       if (v1.screen_x < 0 || v1.screen_x >= xres) return;
       else if (v1.screen_y < 0 || v1.screen_y >= xres) return;
       else if (v2.screen_x < 0 || v2.screen_x >= xres) return;
       else if (v2.screen_y < 0 || v2.screen_y >= xres) return;
       else{
       int x0, x1, y0, y1;
       if (v1.x < v2.x) {
       //make sure that x0 < x1
        x0 = v1.screen_x;
        y0 = v1.screen_y;
        x1 = v2.screen_x;
        y1 = v2.screen_y;
       } else {
        x0 = v2.screen_x;
        y0 = v2.screen_y;
        x1 = v1.screen_x;
        y1 = v1.screen_y;
      }
       int dx = x1 - x0;
       int dy = y1 - y0;
       int error = 0;
       if (dy > dx && dy > 0) {
          //m > 1
          int x = x0;
          for (int y = y0; y <= y1; y++) {
             fill(x, y, xres, yres, pixels);
             if (2 * (error + dx) < dy) {
                error = error + dx;
             } else {
                error = error + dx - dy;
                x++;
             }
          }
       } else if (dy > 0) {
        // 0 < m <= 1
        int y = y0;
        for (int x = x0; x <= x1; x++) {
          fill(x, y, xres, yres, pixels);
          if (2 * (error + dy) < dx) {
            error = error + dy;
          } else {
            error = error + dy - dx;
            y++;
          }
        }
      } else if (dy > -dx) {
        // -1 < m <= 0
        int y = y0;
        for (int x = x0; x <= x1; x++) {
          fill(x, y, xres, yres, pixels);
          if (2 * (error - dy) < dx) {
            error = error - dy;
          } else {
            error = error - dy - dx;
            y--;
          }
        }
      } else {
        // m <= -1
        int x = x0;
        for (int y = y0; y >= y1; y--) {
          fill(x, y, xres, yres, pixels);
          if (2 * (error - dx) > dy) {
            error = error - dx;
          } else {
            error = error - dx - dy;
            x++;
          }
          }
        }
      }
     }

//connect all the vertices that make up a face of an object
void draw_object(Object obj, vector<int>& pixels, int xres, int yres) {
        for (Face f: obj.faces){
            int v1 = f.v1 - 1;
            int v2 = f.v2 - 1;
            int v3 = f.v3 - 1;
            draw_line(obj.vertices[v1], obj.vertices[v2], pixels, xres, yres);
            draw_line(obj.vertices[v1], obj.vertices[v3], pixels, xres, yres);
            draw_line(obj.vertices[v2], obj.vertices[v3], pixels, xres, yres);
        }
     }
     
//draw all the objects in a scene     
void draw_scene(Scene &s, vector<int>& pixels, int xres, int yres) {
     for (Object obj: s.objects) {
         draw_object(obj, pixels, xres, yres);
     }
}
     
//print ppm to console
void print_ppm(int xres, int yres, vector<int>& pixels) {
    cout << "P3" << endl
       << xres << " " << yres << endl
       << "255" << endl;

    for (int j = 0; j < yres; j++) {
      for (int i = 0; i < xres; i++) {
        if (pixels[j * xres + i] == 1) {
           cout << "255 255 255" << endl;
      } else {
        cout << "0 0 0" << endl;
      }
    }
  }
}

#endif

