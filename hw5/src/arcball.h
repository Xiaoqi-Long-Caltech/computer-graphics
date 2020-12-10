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

/* convert from screen coordinates to NDC coordinates: simply invert NDC_to_screen
*/
float screen_to_NDC(int coord, int res) {
    float ndc = (2.0 * coord) / res - 1.0;
    return ndc;
}

float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

/* unit vector of rotation axis */
Vector3f get_rot_axis(int x0, int y0, int x1, int y1, int xres, int yres) {
    float ndc_x0 = screen_to_NDC(x0, xres);
    float ndc_y0 = -screen_to_NDC(y0, yres);
    float ndc_x1 = screen_to_NDC(x1, xres);
    float ndc_y1 = -screen_to_NDC(y1, yres);
    float ndc_z0 = 1 - pow(ndc_x0, 2) - pow(ndc_y0, 2);
    ndc_z0 = (ndc_z0 > 0) ? sqrt(ndc_z0) : 0;
    float ndc_z1 = 1 - pow(ndc_x1, 2) - pow(ndc_y1, 2);
    ndc_z1 = (ndc_z1 > 0) ? sqrt(ndc_z1) : 0;
    //unit rotation axis
    Vector3f ndc_v0(ndc_x0, ndc_y0, ndc_z0);
    Vector3f ndc_v1(ndc_x1, ndc_y1, ndc_z1);
    Vector3f uni_rot_axis = ndc_v0.cross(ndc_v1);
    if (uni_rot_axis.norm() != 0)
    	uni_rot_axis.normalize();
    return uni_rot_axis;
}

/* rotation angle */
float get_rot_angle(int x0, int y0, int x1, int y1, int xres, int yres) {
    float ndc_x0 = screen_to_NDC(x0, xres);
    float ndc_y0 = -screen_to_NDC(y0, yres);
    float ndc_x1 = screen_to_NDC(x1, xres);
    float ndc_y1 = -screen_to_NDC(y1, yres);
    float ndc_z0 = 1 - pow(ndc_x0, 2) - pow(ndc_y0, 2);
    ndc_z0 = (ndc_z0 > 0) ? sqrt(ndc_z0) : 0;
    float ndc_z1 = 1 - pow(ndc_x1, 2) - pow(ndc_y1, 2);
    ndc_z1 = (ndc_z1 > 0) ? sqrt(ndc_z1) : 0;
    //rotation angle
    Vector3f ndc_v0(ndc_x0, ndc_y0, ndc_z0);
    Vector3f ndc_v1(ndc_x1, ndc_y1, ndc_z1);
    float prod = ndc_v0.dot(ndc_v1);
    prod /= (ndc_v0.norm()) * (ndc_v1.norm());
    return acos(min((float)1.0, prod));
}

/* get rotation quaternion given coordinates */
Quaternionn get_rot_quaternion(int x0, int y0, int x1, int y1, int xres, int yres) {
    float rot_angle = get_rot_angle(x0, y0, x1, y1, xres, yres);
    Vector3f rot_axis = get_rot_axis(x0, y0, x1, y1, xres, yres);
    float real = cos(rot_angle / 2);
    float im_scalar = sin(rot_angle / 2);
    Vector3f image = rot_axis * im_scalar;
    Quaternionn q(real, image(0), image(1), image(2));
    return q;
}
