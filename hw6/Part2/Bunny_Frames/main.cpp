#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include "parse_frame.h"
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Animation an;
// Similar to the I-Bar program, use these four variables to keep track of p_{i-1} to p_{i+2}
KeyFrame last;
KeyFrame curr;
KeyFrame next1;
KeyFrame next2;

int frame_i = 0;
int keyframe_i = 0;
float t = 0.0;
float s = 0.5 * (1.0 - t);

//use keyframe 0 for both the first and second control points
void init() {
    vector<KeyFrame> keyframes = an.frames;
    last = keyframes[0];
    curr = keyframes[0];
    next1 = keyframes[1];
    next2 = keyframes[2];
}

void update() {
    frame_i += 1;
    //if the next frame is a keyframe
   if (frame_i == next1.num) {
      vector<KeyFrame> keyframes = an.frames;
      int kf_size = keyframes.size();
      keyframe_i = keyframe_i + 1;
      //update the four control points
      last = keyframes[keyframe_i - 1];
      curr = keyframes[keyframe_i];
      next1 = keyframes[keyframe_i + 1];
      if (keyframe_i + 2 > kf_size - 1)
          next2 = keyframes[keyframe_i + 1];
      else 
          next2 = keyframes[keyframe_i + 2];
   }
}

KeyFrame interpolation() {
    vector<Vec3f> vertices;
    if (frame_i == curr.num) {
       vertices = curr.vertices;
     }
     else {
       // define basis matrix
       Matrix4f B;
       B << 0, 1, 0, 0,
            -s, 0, s, 0,
            2 * s, s - 3, 3 - 2 * s, -s,
            -s, 2 - s, s - 2, s;
            
       // calculate current u
       float gap;
       if (next1.num > curr.num) {
          gap = next1.num - curr.num;
       }
       else {
          gap = an.frame_num - curr.num;
       }
       float u = (frame_i - curr.num) / gap;
       Vector4f u_vec(1, u, u * u, u * u * u);
       
       // interpolate all vertices of current frame
       for (size_t i = 0; i < curr.vertices.size(); i++) {
          Vec3f new_vec;
          Vector4f v_x (last.vertices[i].x, curr.vertices[i].x, next1.vertices[i].x, next2.vertices[i].x);
          Vector4f v_y (last.vertices[i].y, curr.vertices[i].y, next1.vertices[i].y, next2.vertices[i].y);
          Vector4f v_z (last.vertices[i].z, curr.vertices[i].z, next1.vertices[i].z, next2.vertices[i].z);
          new_vec.x = u_vec.dot(B * v_x);
          new_vec.y = u_vec.dot(B * v_y);
          new_vec.z = u_vec.dot(B * v_z);
          vertices.push_back(new_vec);
       }
    }
    KeyFrame frame;
    frame.vertices = vertices;
    frame.faces = curr.faces;
    return frame;
}

int main() {
    an = parse_all();
    init();
    vector<KeyFrame> all_frames;
    for (int i = 0; i <= an.frame_num; i++) {
      KeyFrame frame = interpolation();
      all_frames.push_back(frame);
      if (i < an.frame_num - 1)
         update();
    }
    for (int i = 0; i <= an.frame_num; i++) {
      string filename = "output/";
      if ( (1 <= i && i <= 4) || (6 <= i && i <= 9)) {
         filename += "bunny0" + to_string(i) + ".obj";
         ofstream out(filename);
         out << all_frames[i].to_string();
         out.close();
      }
      else if ((11 <= i && i <= 14) || (16 <= i && i <= 19)) {
         filename += "bunny" + to_string(i) + ".obj";
         ofstream out(filename);
         out << all_frames[i].to_string();
         out.close();
      }
      else continue;
    }
}
