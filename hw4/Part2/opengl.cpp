#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#define _USE_MATH_DEFINES
#define GL_GLEXT_PROTOTYPES 1
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <cstdlib>
#include <eigen3/Eigen/Dense>

#include "parse_scene.h"
#include "arcball.h"
using namespace std;

/* initialize variables and functions, copied from OpenGL_demo.cpp */
Scene s;
void init(void);
void reshape(int width, int height);
void display(void);
float deg2rad(float angle);

void init_lights();
void set_lights();
void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

/*Quaternion objects used to keep track of rotation matrices. */
Quaternionn last_rot;
Quaternionn curr_rot;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;
int xres, yres;

bool is_pressed = false;
bool wireframe_mode = false;

/* variables and functions for shading and texturing */
void readShaders();
extern GLenum readpng(const char *filename);
GLuint displayList;
GLenum shaderProgram;
string vertProgFileName, fragProgFileName;
GLenum colorTex, normalTex;
GLint colorUniformPos, normalUniformPos;
GLint sizeUniformPos, shadingToggle, camUniformPos;

char *color_filename;
char *normal_filename;

float toggle = 1.0;
const float size = 5.0;

/* From here on are all the function implementations.
 */
 
/* implementation of readShaders() function. We set up shaders that override some parts of the default OpenGL pipeline. */
void readShaders() {
   string vertProgramSource, fragProgramSource;
   
   ifstream vertProgFile(vertProgFileName.c_str());
   if (! vertProgFile)
      cerr << "Error opening vertex shader program\n";
   ifstream fragProgFile(fragProgFileName.c_str());
   if (! fragProgFile)
      cerr << "Error opening fragment shader program\n";

   getline(vertProgFile, vertProgramSource, '\0');
   const char* vertShaderSource = vertProgramSource.c_str();

   getline(fragProgFile, fragProgramSource, '\0');
   const char* fragShaderSource = fragProgramSource.c_str();

   char buf[1024];
   GLsizei blah;

   // Initialize shaders
   GLenum vertShader, fragShader;

   shaderProgram = glCreateProgram();

   vertShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertShader, 1, &vertShaderSource, NULL);
   glCompileShader(vertShader);
    
   GLint isCompiled = 0;
   glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isCompiled);
   if(isCompiled == GL_FALSE)
   {
      GLint maxLength = 0;
      glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);
    
      // The maxLength includes the NULL character
      std::vector<GLchar> errorLog(maxLength);
      glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);
    
      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      for (int i = 0; i < errorLog.size(); i++)
         cout << errorLog[i];
      glDeleteShader(vertShader); // Don't leak the shader.
      return;
   }

   fragShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragShader, 1, &fragShaderSource, NULL);
   glCompileShader(fragShader);

   isCompiled = 0;
   glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isCompiled);
   if(isCompiled == GL_FALSE)
   {
      GLint maxLength = 0;
      glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);
    
      // The maxLength includes the NULL character
      std::vector<GLchar> errorLog(maxLength);
      glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);
    
      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      for (int i = 0; i < errorLog.size(); i++)
         cout << errorLog[i];
      glDeleteShader(fragShader); // Don't leak the shader.
      return;
   }
   
   glAttachShader(shaderProgram, vertShader);
   glAttachShader(shaderProgram, fragShader);
   glLinkProgram(shaderProgram);
   cerr << "Enabling fragment program: " << gluErrorString(glGetError()) << endl;
   glGetProgramInfoLog(shaderProgram, 1024, &blah, buf);
   cerr << buf;

   cerr << "Enabling program object" << endl;
   glUseProgram(shaderProgram);

   camUniformPos = glGetUniformLocation(shaderProgram, "cam");
   colorUniformPos = glGetUniformLocation(shaderProgram, "colorTex");
   normalUniformPos = glGetUniformLocation(shaderProgram, "normalTex");
   sizeUniformPos = glGetUniformLocation(shaderProgram, "size");
   shadingToggle = glGetUniformLocation(shaderProgram, "toggle");
   glBindAttribLocation(shaderProgram, 1, "tangent");

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, normalTex);
   glUniform1i(normalUniformPos, 0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, colorTex);
   glUniform1i(colorUniformPos, 1);
}
 
/* init function slightly altered from demo */
void init(void)
{
    //deleted default shading
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(s.left, s.right,
              s.bottom, s.top,
              s.near, s.far);
              
    glMatrixMode(GL_MODELVIEW);
    
    /* initialize rotation matrices as identities; default constructor for Quaternionn returns the identity */
    last_rot = Quaternionn();
    curr_rot = Quaternionn();
    //copied from demo
    GLfloat pos[] = {7.0, 2.0, 3.0, 1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_DIFFUSE);
    GLfloat black[4] = {0,0,0,0};
    glMaterialfv(GL_FRONT,GL_AMBIENT,black);
    glMaterialfv(GL_FRONT,GL_SPECULAR,black);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    
    cerr << "Loading textures" << endl;
    if(!(colorTex = readpng(color_filename)))
        exit(1);
    if(!(normalTex = readpng(normal_filename)))
        exit(1);
}


/* reshape function from demo */
void reshape(int width, int height)
{
    
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;
    
    glViewport(0, 0, width, height);
    
    mouse_scale_x = (float) (s.right - s.left) / (float) width;
    mouse_scale_y = (float) (s.top - s.bottom) / (float) height;
    
    glutPostRedisplay();
}

/* display function from demo */
void display(void)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glColor3f(1,0,0);
    glUseProgram(shaderProgram);
    
    glUniform1f(sizeUniformPos, size);
    glUniform1f(shadingToggle, toggle);
    Vector3f cam = get_cam_position(s);
    glUniform3f(camUniformPos, cam(0), cam(1), cam(2));
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
     
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);
    
    glRotatef(-deg2rad(s.orientation_angle_cam),
              s.orientation_camera.x, s.orientation_camera.y, s.orientation_camera.z);
    glTranslatef(-s.position_camera.x, -s.position_camera.y, -s.position_camera.z);
    
    //calculate current rotation matrix and store in glMultMatrixf()
    auto arcball_m = (curr_rot * last_rot).GetRotationMatrix();
    glMultMatrixf(&arcball_m[0]);
    
    //hardcode a square, referencing to method in demo
    glColor3f(0, 1, 0);
    
    glBegin(GL_TRIANGLE_STRIP);
    
        glTexCoord2f(0, 0);
        glNormal3f(0.0, 0.0, 1.0);
        glVertexAttrib3f(1, 1.0, 0.0, 0.0);
        glVertex2f(-2.5, -2.5);

        glTexCoord2f(1, 0);
        glNormal3f(0.0, 0.0, 1.0);
        glVertexAttrib3f(1, 1.0, 0.0, 0.0);
        glVertex2f(2.5, -2.5);

        glTexCoord2f(0, 1);
        glNormal3f(0.0, 0.0, 1.0);
        glVertexAttrib3f(1, 1.0, 0.0, 0.0);
        glVertex2f(-2.5, 2.5);

        glTexCoord2f(1, 1);
        glNormal3f(0.0, 0.0, 1.0);
        glVertexAttrib3f(1, 1.0, 0.0, 0.0);
        glVertex2f(2.5, 2.5);
        
    glEnd();
    
    glPopMatrix();
    glutSwapBuffers();
}

void mouse_pressed(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouse_x = x;
        mouse_y = y;
        
        is_pressed = true;
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        /* Update the rotation Quaternions
         */
        last_rot = curr_rot * last_rot;
        curr_rot = Quaternionn(); //identity quaternion
        is_pressed = false;
    }
}

void mouse_moved(int x, int y)
{
    /* If the left-mouse button is being clicked down...
     */
    if(is_pressed)
    {
        curr_rot = get_rot_quaternion(mouse_x, mouse_y, x, y, s.xres, s.yres);
        //calculate the new rotation quaternion using the location of the mouse
        glutPostRedisplay();
    }
}

/* 'deg2rad' function:
 * 
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

void key_pressed(unsigned char key, int x, int y)
{
    /* If 'q' is pressed, quit the program.
     */
    if(key == 'q')
    {
        exit(0);
    }
}

/* The 'main' function:
 *
 * This function is short, but is basically where everything comes together.
 */
 
 
 int main(int argc, char* argv[])
{
    
    s.xres = 500;
    s.yres = 500;
    
    color_filename = argv[1];
    normal_filename = argv[2];
    
    //hardcode scene attributes
    s.position_camera.SetX(0);
    s.position_camera.SetY(0);
    s.position_camera.SetZ(7);
    s.orientation_camera.SetX(1);
    s.orientation_camera.SetY(0);
    s.orientation_camera.SetZ(0);
    s.orientation_angle_cam = 0;
    s.near = 1;
    s.far = 10;
    s.left = -0.5;
    s.right = 0.5;
    s.top = 0.5;
    s.bottom = -0.5;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(s.xres, s.yres);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");
    
    init();
    
    vertProgFileName = "vertexProgram.glsl";
    fragProgFileName = "fragmentProgram.glsl";
    readShaders();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_pressed);
    glutMotionFunc(mouse_moved);
    glutKeyboardFunc(key_pressed);
    glutMainLoop();
}
