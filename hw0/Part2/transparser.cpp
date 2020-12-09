#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
using namespace Eigen;
using namespace std;

/**construct the scaling matrix from the scaling vector */
MatrixXd get_scale(float sx, float sy, float sz) {

    MatrixXd m(4,4);
    
    m << sx, 0, 0, 0,
      0, sy, 0, 0,
      0, 0, sz, 0,
      0, 0, 0, 1;
      
    return m;
}


/** construct the translation matrix from the translation vector */
MatrixXd get_trans(float tx, float ty, float tz) {
   
    MatrixXd m(4,4);
    
    m << 1, 0, 0, tx,
      0, 1, 0, ty,
      0, 0, 1, tz,
      0, 0, 0, 1;
    
    return m;
}

/** construct the rotation matrix from the rotation vector */
MatrixXd get_rot(float rx, float ry, float rz, float angle) {

    MatrixXd m = MatrixXd::Zero(4, 4);
    m(0, 0) = pow(rx, 2) + (1 - pow(rx, 2)) * cos(angle);
    m(0, 1) = rx * ry * (1 - cos(angle)) - rz * sin(angle);
    m(0, 2) = rx * rz * (1 - cos(angle)) + ry * sin(angle);
    m(1, 0) = ry * rx * (1 - cos(angle)) + rz * sin(angle);
    m(1, 1) = pow(ry, 2) + (1 - pow(ry, 2)) * cos(angle);
    m(1, 2) = ry * rz * (1 - cos(angle)) - rx * sin(angle);
    m(2, 0) = rz * rx * (1 - cos(angle)) - ry * sin(angle);
    m(2, 1) = rz * ry * (1 - cos(angle)) + rx * sin(angle);
    m(2, 2) = pow(rz, 2) + (1 - pow(rz, 2)) * cos(angle);
    m(3, 3) = 1;
    
    return m;

}



int main(int argc, const char *argv[]) {
    MatrixXd m = MatrixXd::Identity(4, 4);
    
    assert(argc == 2);
    ifstream infile(argv[1]);
    char t;
    
    while (infile >> t) {
         MatrixXd m_next;
         if (t == 't') {
             float tx, ty, tz;
             infile >> tx >> ty >> tz;
             m_next = get_trans(tx, ty, tz);
         }
         else if (t == 'r') {
             float rx, ry, rz, angle;
             infile >> rx >> ry >> rz >> angle;
             m_next = get_rot(rx, ry, rz, angle);
         }
         else if (t == 's') {
             float sx, sy, sz;
             infile >> sx >> sy >> sz;
             m_next = get_scale(sx, sy, sz);
         }
         
         m = m_next * m; //When multiplying A, B, and C, we actually want CBA.
         
    }
    MatrixXd m_inv = m.inverse();
    cout << m_inv << endl;
}


