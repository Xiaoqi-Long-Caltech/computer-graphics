#include <stdio.h>
#include <iostream>
#include <fstream>
#include "parse_scene.h"
#include "arcball.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <math.h>
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>

using namespace std;

/* initialize variables and functions, copied from OpenGL_demo.cpp */
Scene s;
void init(void);
void reshape(int width, int height);
void display(void);

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

bool is_pressed = false;
bool wireframe_mode = false;

/* From here on are all the function implementations.
 */
 
/* init function slightly altered from demo */
void init(void)
{
    glShadeModel(GL_SMOOTH);
    
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
    
    init_lights();
    /* initialize rotation matrices as identities; default constructor for Quaternionn returns the identity */
    last_rot = Quaternionn();
    curr_rot = Quaternionn();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
     
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);
    
    glRotatef(-s.orientation_angle_cam,
              s.orientation_camera.x, s.orientation_camera.y, s.orientation_camera.z);
    glTranslatef(-s.position_camera.x, -s.position_camera.y, -s.position_camera.z);
    
    //calculate current rotation matrix and store in glMultMatrixf()
    auto arcball_m = (curr_rot * last_rot).GetRotationMatrix();
    glMultMatrixf(&arcball_m[0]);
    set_lights();
    
    draw_objects();
    
    glutSwapBuffers();
}

/* initialize lights as in demo */
void init_lights()
{
    glEnable(GL_LIGHTING);
    
    int num_lights = s.lights.size();
    
    for(int i = 0; i < num_lights; ++i)
    {
        int light_id = GL_LIGHT0 + i;
        
        glEnable(light_id);
        
        glLightfv(light_id, GL_AMBIENT, s.lights[i].color);
        glLightfv(light_id, GL_DIFFUSE, s.lights[i].color);
        glLightfv(light_id, GL_SPECULAR, s.lights[i].color);
        
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, s.lights[i].attenuation_k);
    }
}

void set_lights()
{
    int num_lights = s.lights.size();
    
    for(int i = 0; i < num_lights; ++i)
    {
        int light_id = GL_LIGHT0 + i;
        
        glLightfv(light_id, GL_POSITION, s.lights[i].position);
    }
}

void draw_objects()
{
    int num_objects = s.objects.size();
    
    for(int i = 0; i < num_objects; ++i)
    {
        glPushMatrix();
        {
            int num_transform_sets = s.objects[i].transform_sets.size();
             
            for(int j = num_transform_sets - 1; j >= 0; --j)
            {
                glTranslatef(s.objects[i].transform_sets[j].translation[0],
                             s.objects[i].transform_sets[j].translation[1],
                             s.objects[i].transform_sets[j].translation[2]);
                glRotatef(s.objects[i].transform_sets[j].rotation_angle,
                          s.objects[i].transform_sets[j].rotation[0],
                          s.objects[i].transform_sets[j].rotation[1],
                          s.objects[i].transform_sets[j].rotation[2]);
                glScalef(s.objects[i].transform_sets[j].scaling[0],
                         s.objects[i].transform_sets[j].scaling[1],
                         s.objects[i].transform_sets[j].scaling[2]);
            }
            
            glMaterialfv(GL_FRONT, GL_AMBIENT, s.objects[i].ambient_reflect);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, s.objects[i].diffuse_reflect);
            glMaterialfv(GL_FRONT, GL_SPECULAR, s.objects[i].specular_reflect);
            glMaterialf(GL_FRONT, GL_SHININESS, s.objects[i].shininess);
            
            glVertexPointer(3, GL_FLOAT, 0, &s.objects[i].vertex_buffer[0]);
            glNormalPointer(GL_FLOAT, 0, &s.objects[i].normal_buffer[0]);
            
            int buffer_size = s.objects[i].vertex_buffer.size();
            
            if(!wireframe_mode)
                glDrawArrays(GL_TRIANGLES, 0, buffer_size);
            else
                for(int j = 0; j < buffer_size; j += 3)
                    glDrawArrays(GL_LINE_LOOP, j, 3);
        }
        glPopMatrix();
    }
    
    glPushMatrix();
    {
        glTranslatef(0, -103, 0);
        glutSolidSphere(100, 100, 100);
    }
    glPopMatrix();
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
    /* If 't' is pressed, toggle our 'wireframe_mode' boolean to make OpenGL
     * render our cubes as surfaces of wireframes.
     */
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        glutPostRedisplay();
    }
    else
    {
        /* These might look a bit complicated, but all we are really doing is
         * using our current change in the horizontal camera angle (ie. the
         * value of 'x_view_angle') to compute the correct changes in our x and
         * z coordinates in camera space as we move forward, backward, to the left,
         * or to the right.
         *
         * 'step_size' is an arbitrary value to determine how "big" our steps
         * are.
         *
         * We make the x and z coordinate changes to the camera position, since
         * moving forward, backward, etc is basically just shifting our view
         * of the scene.
         */
        
        float x_view_rad = deg2rad(x_view_angle);
        
        /* 'w' for step forward
         */
        if(key == 'w')
        {
            s.position_camera.x += step_size * sin(x_view_rad);
            s.position_camera.z -= step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        /* 'a' for step left
         */
        else if(key == 'a')
        {
            s.position_camera.x -= step_size * cos(x_view_rad);
            s.position_camera.z -= step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
        /* 's' for step backward
         */
        else if(key == 's')
        {
            s.position_camera.x -= step_size * sin(x_view_rad);
            s.position_camera.z += step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        /* 'd' for step right
         */
        else if(key == 'd')
        {
            s.position_camera.x += step_size * cos(x_view_rad);
            s.position_camera.z += step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
    }
}

/* The 'main' function:
 *
 * This function is short, but is basically where everything comes together.
 */
 
 int main(int argc, char* argv[])
{
    
    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    
    s = parse_camera(infile);
    s.xres = xres;
    s.yres = yres;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(xres, yres);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");
    
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_pressed);
    glutMotionFunc(mouse_moved);
    glutKeyboardFunc(key_pressed);
    glutMainLoop();
}
