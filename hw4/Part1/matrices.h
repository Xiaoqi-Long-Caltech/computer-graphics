#ifndef MATRICES_H
#define MATRICES_H

#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

/** Integrate the program in HW0 Part 2 into a single header file. Added function to compute the product of a vector of matrices. */

using namespace Eigen;
using namespace std;

/**construct the scaling matrix from the scaling vector */
Matrix4f get_scale(float sx, float sy, float sz) {

    Matrix4f m;
    
    m << 
      sx, 0, 0, 0,
      0, sy, 0, 0,
      0, 0, sz, 0,
      0, 0, 0, 1;
      
    return m;
}


/** construct the translation matrix from the translation vector */
Matrix4f get_trans(float tx, float ty, float tz) {
   
    Matrix4f m;
    
    m << 
      1, 0, 0, tx,
      0, 1, 0, ty,
      0, 0, 1, tz,
      0, 0, 0, 1;
    
    return m;
}

/** construct the rotation matrix from the rotation vector */
Matrix4f get_rot(float rx, float ry, float rz, float angle) {
    float magn = sqrt(rx * rx + ry * ry + rz * rz);
    rx = rx/magn;
    ry = ry/magn;
    rz = rz/magn;
    Matrix4f m;
    m(0, 0) = pow(rx, 2) + (1 - pow(rx, 2)) * cos(angle);
    m(0, 1) = rx * ry * (1 - cos(angle)) - rz * sin(angle);
    m(0, 2) = rx * rz * (1 - cos(angle)) + ry * sin(angle);
    m(0, 3) = 0;
    m(1, 0) = ry * rx * (1 - cos(angle)) + rz * sin(angle);
    m(1, 1) = pow(ry, 2) + (1 - pow(ry, 2)) * cos(angle);
    m(1, 2) = ry * rz * (1 - cos(angle)) - rx * sin(angle);
    m(1, 3) = 0;
    m(2, 0) = rz * rx * (1 - cos(angle)) - ry * sin(angle);
    m(2, 1) = rz * ry * (1 - cos(angle)) + rx * sin(angle);
    m(2, 2) = pow(rz, 2) + (1 - pow(rz, 2)) * cos(angle);
    m(2, 3) = 0;
    m(3, 0) = 0;
    m(3, 1) = 0;
    m(3, 2) = 0;
    m(3, 3) = 1;
    
    return m;

}

/** for input in the order A, B, C, we want the product CBA. */
Matrix4f get_product(vector<Matrix4f> matrices) {
    Matrix4f output = matrices[0];
    for (int i = 1; i < matrices.size(); i++) {
    	output = matrices[i] * output;
    }
    return output;
    
}

#endif

