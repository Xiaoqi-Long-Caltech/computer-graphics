#include <stdio.h>
#include <eigen3/Eigen/Dense>
#include "rasterize.h"
#include "transform.h"

using namespace std;
using Eigen::Matrix4f;

/**main function that integrates the program**/

int main(int argc, const char* argv[]) {
    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    Scene s = parse_camera(infile); //parse scene
    
    vector<Object> new_objects; //attempt to apply object transformations
    for (Object obj : s.objects){
        geometric_trans(obj);
        camera_trans(obj, s);
        NDC_trans(obj, s);
        new_objects.push_back(obj);
    }
    s.SetObjects(new_objects); //update objects in scene
    NDC_to_screen(s, xres, yres); //convert to screen coordinates
    vector<int> pixels(yres * xres); //create canvas
    draw_scene(s, pixels, xres, yres); //fill in pixels
    print_ppm(xres, yres, pixels); //print ppm
    return 0;
}

