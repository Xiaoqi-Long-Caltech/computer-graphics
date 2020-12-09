#include <stdio.h>
#include <eigen3/Eigen/Dense>
#include "rasterize.h"
#include "transform.h"
#include <cfloat>

using namespace std;
using namespace Eigen;

/**main function that integrates the program**/

int main(int argc, const char* argv[]) {
    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    int mode = atoi(argv[4]);
    Scene s = parse_camera(infile); //parse scene
    
    vector<Object> new_objects; //attempt to apply object transformations
    for (Object obj : s.objects){
        geometric_trans(obj);
        trans_obj_norm(obj);
        new_objects.push_back(obj);
    }
    s.SetObjects(new_objects); //update objects in scene

    MatrixColor grid(yres, xres);
    MatrixXf depth_buffer(yres, xres);
    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            grid(y, x) = Color(0, 0, 0);
        }
    }
    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            depth_buffer(y, x) = DBL_MAX;
        }
    }
    s.depth_buffer = depth_buffer;
    if (mode == 0) {
       for (Object obj : s.objects){
           for(Face f: obj.faces){
              Gouraud_shading(f, obj, s, grid);
           }
       }
    }
    else if (mode == 1) {
      for (Object obj : s.objects){
         for(Face f: obj.faces){
            Phong_shading(f, obj, s, grid);
         }
      }
    }
    print_ppm(xres, yres, grid);
    return 0;
}

