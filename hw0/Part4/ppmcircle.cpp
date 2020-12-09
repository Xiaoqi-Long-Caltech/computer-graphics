#include <cmath>
#include <iostream>
#include <eigen3/Eigen/Dense>

using namespace std;
using Eigen::MatrixXd;

int main(int argc, char *argv[]) {
    int xres = stoi(argv[1]);
    int yres = stoi(argv[2]);
    float radius = min(xres, yres) / 4;
    
    MatrixXd determine_color = MatrixXd::Zero(xres, yres);

    for (int i = 0; i < yres; i++) {
        for (int j = 0; j < xres; j++) {
            float x = i - xres / 2;
            float y = j - yres / 2;
            if (pow(x, 2) + pow(y, 2) <= pow(radius, 2)) {
                determine_color(i, j) = 1;
            }
        }
    }
    
    cout << "P3" << endl;
    cout << xres << " " << yres << endl;
    cout << 255 << endl;
    
    for (int i = 0; i < xres; i++) {
        for (int j = 0; j < yres; j++){
           if (determine_color(i, j) == 1) {
              cout << "176 194 196" << endl; //AeStHeTiC
           }
           else {
              cout << "241 231 227" << endl;
           }
        }
    }
    return 0;
}
