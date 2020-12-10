/* This is a code snippet for drawing the "I-bar". The 'quadratic' object can be made
 * into a global variable in your program if you want. Line 13, where 'quadratic' gets
 * initialized should be done in your 'init' function or somewhere close to the start
 * of your program.
 *
 * The I-bar is an object that Prof. Al Barr once used in one of his papers to demonstrate
 * an interpolation technique. You might call it "Al Barr's I-Bar" ;)
 */

/* Needed to draw the cylinders using glu */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include "parse_frame.h"
#include "quaternion.h"
#include <Eigen/Dense>
#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;
using namespace Eigen;

GLUquadricObj *quadratic;

void init();
void display();
void drawIBar();
void key_pressed(unsigned char key, int x, int y);
void init_frames_of_interest();
void next_frames_of_interest();
void init_lighting();
void init_material();
void interpolation();
void init_transformation();
void update_transformation();

const float light_color[3] = {1, 1, 1};
const float light_position[3] = {0, 0, -2};
const float ambient_reflect[3] = {0.1, 0.1, 0.1};
const float diffuse_reflect[3] = {0.2, 0.2, 0.2};
const float specular_reflect[3] = {0.3, 0.3, 0.3};
const float shininess = 0.1;

Animation an;
int frame_i = 0;
int keyframe_i = 0;
Vec3f trans(0, 0, 0);
Vec3f scale(1, 1, 1);
Vec4f rot(1, 0, 0, 0);
//t and s are specified for Catmull-Rom splines
float t = 0.0;
float s = 0.5 * (1.0 - t);

//used to keep track of p_{i-1}, p_i, p_{i+1}, p_{i+2}
Frame last;
Frame curr;
Frame next1;
Frame next2;

int main(int argc, char* argv[]) {
   ifstream infile(argv[1]);
   int xres = atoi(argv[2]);
   int yres = atoi(argv[3]);
   an = parse(infile);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(xres, yres);
   glutInitWindowPosition(0, 0);
   glutCreateWindow("Test");
   init();
   glutDisplayFunc(display);
   glutKeyboardFunc(key_pressed);
   glutMainLoop();
}

void init()
{
    quadratic = gluNewQuadric();
    init_frames_of_interest(); //assign the first few frames to last, curr, next1, next2
    init_transformation(); //keep track of current transformation parameters
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 60.0);
    glMatrixMode(GL_MODELVIEW);
    
    init_lighting();
    init_material();
}

/** hardcode lighting parameters **/
void init_lighting()
{   
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);

}

/** hardcode material parameters **/
void init_material()
{
   glEnable(GL_COLOR_MATERIAL);
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_reflect);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_reflect);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflect);
   glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

/** assign the first few frames to last, curr, next1, next2 **/
void init_frames_of_interest() {
   vector<Frame> keyframes = an.frames;
   last = keyframes[keyframes.size() - 1];
   curr = keyframes[0];
   next1 = keyframes[1];
   next2 = keyframes[2];
}

/** keep track of current transformation parameters **/
void init_transformation() {
   trans = curr.translation;
   scale = curr.scale;
   rot = curr.rotation;
}

/** update the assignments of last, curr, next1, next2 **/
void next_frames_of_interest() {
   frame_i = (frame_i + 1) % an.frame_num;
   //if the next frame is a keyframe
   if (frame_i == next1.num) {
      vector<Frame> keyframes = an.frames;
      int kf_size = keyframes.size();
      keyframe_i = (keyframe_i + 1) % kf_size;
      last = keyframes[(keyframe_i - 1 + kf_size) % kf_size]; // added additional kf_size to avoid problems when the index becomes negative
      curr = keyframes[keyframe_i];
      next1 = keyframes[(keyframe_i + 1) % kf_size];
      next2 = keyframes[(keyframe_i + 2) % kf_size];
   }
   // if it is not a keyframe, just increase (and mod) the frame_i.
}

void interpolation() {
     if (frame_i == curr.num) {
       trans = curr.translation;
       scale = curr.scale;
       rot = curr.rotation;
     }
     else {
       // define the basis matrix
       Matrix4f B;
       B << 0, 1, 0, 0,
            -s, 0, s, 0,
            2 * s, s - 3, 3 - 2 * s, -s,
            -s, 2 - s, s - 2, s;
       // Calculate u for the current step
       float gap;
       if (next1.num > curr.num) {
          gap = next1.num - curr.num;
       }
       else {
          gap = an.frame_num - curr.num;
       }
       float u = (frame_i - curr.num) / gap;
       
       // define vectors
       Vector4f u_vec(1, u, u * u, u * u * u);
       
       Vector4f t_x (last.translation.x, curr.translation.x, next1.translation.x, next2.translation.x);
       Vector4f t_y (last.translation.y, curr.translation.y, next1.translation.y, next2.translation.y);
       Vector4f t_z (last.translation.z, curr.translation.z, next1.translation.z, next2.translation.z);
       // interpolation
       trans.x = u_vec.dot(B * t_x);
       trans.y = u_vec.dot(B * t_y);
       trans.z = u_vec.dot(B * t_z);
       
       Vector4f s_x (last.scale.x, curr.scale.x, next1.scale.x, next2.scale.x);
       Vector4f s_y (last.scale.y, curr.scale.y, next1.scale.y, next2.scale.y);
       Vector4f s_z (last.scale.z, curr.scale.z, next1.scale.z, next2.scale.z);
       // interpolation
       scale.x = u_vec.dot(B * s_x);
       scale.y = u_vec.dot(B * s_y);
       scale.z = u_vec.dot(B * s_z);
       
       Quaternionn last_quat;
       Quaternionn curr_quat;
       Quaternionn next1_quat;
       Quaternionn next2_quat;
       // transform rotation to quaternions
       last_quat = get_rot_quaternion(last.rotation);
       curr_quat = get_rot_quaternion(curr.rotation);
       next1_quat = get_rot_quaternion(next1.rotation);
       next2_quat = get_rot_quaternion(next2.rotation);
       // create vectors of quaternions
       Vector4f r_s (last_quat.s, curr_quat.s, next1_quat.s, next2_quat.s);
       Vector4f r_x (last_quat.x, curr_quat.x, next1_quat.x, next2_quat.x);
       Vector4f r_y (last_quat.y, curr_quat.y, next1_quat.y, next2_quat.y);
       Vector4f r_z (last_quat.z, curr_quat.z, next1_quat.z, next2_quat.z);
       float new_s, new_x, new_y, new_z;
       // interpolate quaternions
       new_s = u_vec.dot(B * r_s);
       new_x = u_vec.dot(B * r_x);
       new_y = u_vec.dot(B * r_y);
       new_z = u_vec.dot(B * r_z);
       Quaternionn new_quat(new_s, new_x, new_y, new_z);
       //transform back to rotation
       Vec4f new_rot = Q2rot(new_quat);
       rot.x = new_rot.x;
       rot.y = new_rot.y;
       rot.z = new_rot.z;
       rot.a = new_rot.a;

     }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glLoadIdentity();
    glTranslatef(-0, -0 ,-40);   
    drawIBar();   
    glutSwapBuffers();
}

void update_transformation() {
    glTranslatef(trans.x, trans.y, trans.z);
    glRotatef(rot.a, rot.x, rot.y, rot.z);
    glScalef(scale.x, scale.y, scale.z);
}

void drawIBar()
{
    /* Parameters for drawing the cylinders */
    float cyRad = 0.2, cyHeight = 1.0;
    int quadStacks = 4, quadSlices = 4;
    
    glPushMatrix();
    update_transformation();
    glColor3f(0, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quadratic, cyRad, cyRad, 2.0 * cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    update_transformation();
    glColor3f(0, 1, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    update_transformation();
    glColor3f(1, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    update_transformation();
    glColor3f(1, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
    
    glPushMatrix();
    update_transformation();
    glColor3f(0, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
}

void key_pressed(unsigned char key, int x, int y) {
   if (key == 'q') {
        exit(0);
    } else if (key == 's') {
        next_frames_of_interest();
        cout << "Frame number " << frame_i << endl;
        interpolation();
        glutPostRedisplay();
    }
}
