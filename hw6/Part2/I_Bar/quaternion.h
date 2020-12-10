#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

/*Struct for quaternions. Named them with double n so as to avoid confusion with class in Eigen.
*/
struct Quaternionn {
   float s, x, y, z;
   Quaternionn() : s(1), x(0), y(0), z(0) {} //creates a quaternion identity
   Quaternionn(float s_, float x_, float y_, float z_)
      : s(s_), x(x_), y(y_), z(z_) {}
      
   //define multiplication for quaternions
   void operator*=(const Quaternionn& q) {
      float new_s = s * q.s - this->vdot(q);
      float new_x = s * q.x + q.s * x + (y * q.z - z * q.y);
      float new_y = s * q.y + q.s * y + (z * q.x - x * q.z);
      float new_z = s * q.z + q.s * z + (x * q.y - y * q.x);
      s = new_s;
      x = new_x;
      y = new_y;
      z = new_z;
   }
   
   Quaternionn operator*(const Quaternionn& q) {
      Quaternionn res = *this;
      res *= q;
      return res;
   }
   
   float vdot(const Quaternionn& q) const {
      return x * q.x + y * q.y + z * q.z;
   }
   
   void normalize() {
      float norm = sqrt(s * s + x * x + y * y + z * z);
      s = s / norm;
      x = x / norm;
      y = y / norm;
      z = z / norm;
   }
   
   /** Get rotation matrix from quaternion. For some reason, I had to alter some signs in this matrix from the lecture notes to get the object rotate in an intuitive manner. **/
   
   array<float, 16> GetRotationMatrix() {
      array<float, 16> res;
      // col 1
      res[0] =  1 - 2 * y * y - 2 * z * z;
      res[1] =  2 * (x * y + z * s);
      res[2] =  2 * (x * z - y * s);
      res[3] =  0;
      
      // col 2
      res[4] = 2 * (x * y - z * s);
      res[5] = 1 - 2 * x * x - 2 * z * z;
      res[6] = 2 * (y * z + x * s);
      res[7] = 0;
      
      // col 3
      res[8] =  2 * (x * z + y * s);
      res[9] = 2 * (y * z - x * s);
      res[10] = 1 - 2 * x * x - 2 * y * y;
      res[11] = 0;
      
      // col 4
      res[12] = 0;
      res[13] = 0;
      res[14] = 0;
      res[15] = 1;
       return res;
   }
   
};


float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

float rad2deg(float angle)
{
    return angle * 180.0 / M_PI;
}
/* get rotation quaternion given rotation */
Quaternionn get_rot_quaternion(Vec4f rotation) {
    float rot_angle = deg2rad(rotation.a);
    Vector3f rot_axis(rotation.x, rotation.y, rotation.z);
    rot_axis.normalize();
    float real = cos(rot_angle / 2);
    float im_scalar = sin(rot_angle / 2);
    Vector3f image = rot_axis * im_scalar;
    Quaternionn q(real, image(0), image(1), image(2));
    return q;
}

Vec4f Q2rot(Quaternionn q) {
    q.normalize();
    float rot_angle = 2.0 * acos(q.s);
    if (rot_angle == 0) {
       Vec4f res(1, 0, 0, 0);
       return res;
    }
    else {
       float sine = sin(rot_angle / 2);
       float x = q.x / sine;
       float y = q.y / sine;
       float z = q.z / sine;
       Vector3f rot_axis(x, y, z);
       Vec4f res(rot_axis(0), rot_axis(1), rot_axis(2), rad2deg(rot_angle));
       return res;
   }
}
