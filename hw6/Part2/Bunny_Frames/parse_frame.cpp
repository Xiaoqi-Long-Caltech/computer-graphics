#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include "parse_frame.h"

using namespace std;

// hardcoded filenames for input obj files
const string filenames[] = {"bunny00.obj", "bunny05.obj", "bunny10.obj", "bunny15.obj", "bunny20.obj"};
// hardcode keyframe indices
const int frame_i[] = {0, 5, 10, 15, 20};

/** parse a single keyframe */
KeyFrame parse_keyframe(ifstream &infile) {
      string line;
      vector<Vec3f> vertices;
      vector<Vec3f> faces;
      KeyFrame keyframe;
      
      while(getline(infile, line)) {
          istringstream iss(line);
          char t;
          float x, y, z;
          iss >> t >> x >> y >> z;
          Vec3f vec(x, y, z);
          
          if (t == 'v') {
              vertices.push_back(vec);
          }
          else {
              faces.push_back(vec);
          }
      }
      
      keyframe.vertices = vertices;
      keyframe.faces = faces;
      return keyframe;
}

/** store everything in global structure Animation **/
Animation parse_all() {
     vector<KeyFrame> keyframes;
     Animation an;
     
     for (int i = 0; i < 5; i++) {
        string directory = "keyframes/";
        directory.append(filenames[i]);
        ifstream infile(directory);
        KeyFrame keyframe = parse_keyframe(infile);
        int num = frame_i[i];
        keyframe.num = num;
        keyframes.push_back(keyframe);
     }
     an.frames = keyframes;
     an.frame_num = 20;
     return an;
}
