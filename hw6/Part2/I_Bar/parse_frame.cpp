#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include "parse_frame.h"

Animation parse(ifstream &infile) {
     int frame_num = 0;
     vector<Frame> frames;
     string line;
     
     while(getline(infile, line)) {
         string t;
         istringstream iss(line);
         if (frame_num == 0) {
            iss >> frame_num;
            continue;
         }
         if (line.compare("") == 0)
            continue;
         Frame frame;
         // the next four lines are the parameters for the same keyframe
         int i = 0;
         while (i < 4) {
             iss >> t;
             if (t.compare("Frame") == 0) {
                int x;
                iss >> x;
                frame.num = x;
             }
             else if (t.compare("translation") == 0) {
                int x, y, z;
                iss >> x >> y >> z;
                Vec3f translation(x, y, z);
                frame.translation = translation;
             }
             else if (t.compare("scale") == 0) {
                int x, y, z;
                iss >> x >> y >> z;
                Vec3f scale(x, y, z);
                frame.scale = scale;
             }
             else if (t.compare("rotation") == 0) {
                int x, y, z, a;
                iss >> x >> y >> z >> a;
                Vec4f rotation(x, y, z, a);
                frame.rotation = rotation;
             }
             if (i < 3) {
                 getline(infile, line);
                 iss.clear();
                 iss.str(line);
             }
             i++;
         }
         frames.push_back(frame);       
     }
     Animation an;
     an.frame_num = frame_num;
     an.frames = frames;
     return an;
}
