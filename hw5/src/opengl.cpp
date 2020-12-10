#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#define _USE_MATH_DEFINES
#define GL_GLEXT_PROTOTYPES 1
#include <GL/glew.h>
#include <GL/glut.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <cstdlib>
#include "opengl.h"
#include "parse_scene.h"

using namespace std;

Scene s;
Mesh_Data *mesh_data;
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
bool smoothing_mode = false;

/* From here on are all the function implementations.
 */

/* In this function, we parse the scene from the .txt file and store the objects in two attributes: scene.objects and scene.smooth_objects. With initial parsing, the two vectors
contain the same data, but only the smooth_objects will be smoothed. Depending on the smoothing_mode, we decide whether to draw smooth_objects or just objects. */

Scene parse_camera(ifstream &infile, float h) {
	Scene scene;
	string line, t;
	float x, y, z, angle, p, r, g, b, atten;
	char van;
	//start parsing the file
	while (getline(infile, line)) {
	if (line.compare("objects:") == 0) {
	    //store object data in two separate vectors; one of them will be smoothed.
	    vector<Object> object_copies;
            vector<Object> smooth_object_copies;
            mesh_data = new Mesh_Data;
            parse_objects(infile, mesh_data, object_copies, smooth_object_copies, h, scene); //call parse_objects when reaching objects
            scene.hevs = new vector<HEV*>();
            vector<HEF*> *o_hefs = new vector<HEF*>(); 
            //fill in the hevs and hefs
            build_HE(mesh_data, scene.hevs, o_hefs);
            //fill in the normals into objects
            get_normals(scene.hevs);
            populate_norm(&scene.objects, mesh_data, scene.hevs);
            //fill in the normals into smooth_objects
            scene.smooth_objects = update(scene.hevs, h, mesh_data, scene.smooth_objects);
            break;
        } else {
               //read camera parameters
		istringstream iss(line);
		iss >> t;
		if (t.compare("camera:") == 0) {
			continue;
		}
		else if (t.compare("position") == 0) {
			iss >> x >> y >> z;
			Vec3f position_camera(x, y, z);
			scene.position_camera = position_camera;
		}
		else if (t.compare("orientation") == 0) {
			iss >> x >> y >> z >> angle;
			Vec3f orientation_camera(x, y, z);
			scene.orientation_camera = orientation_camera;
			scene.orientation_angle_cam = angle;
		}
		else if (t.compare("near") == 0) {
			iss >> p;
			scene.near = p;
		}
		else if (t.compare("far") == 0) {
			iss >> p;
			scene.far = p;
		}
		else if (t.compare("left") == 0) {
			iss >> p;
			scene.left = p;
		}
		else if (t.compare("right") == 0) {
			iss >> p;
			scene.right = p;
		}
		else if (t.compare("top") == 0) {
			iss >> p;
			scene.top = p;
		}
		else if (t.compare("bottom") == 0) {
			iss >> p;
			scene.bottom = p;
		}
		else if (t.compare("light") == 0) {
			iss >> x >> y >> z >> van >> r >> g >> b >> van >> atten;
			Point_Light l(x, y, z, 1, r, g, b, atten);
			scene.lights.push_back(l);
		}
	 }
	}
	return scene;
}
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

    glRotatef(rad2deg(-s.orientation_angle_cam),
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

/* As explained above, the draw_objects() function needs to first decide which objects to draw based on the smoothing mode. The objects vector is unsmoothed.*/
void draw_objects()
{
    vector<Object> objets;
    if (smoothing_mode) {
       objets = s.smooth_objects;
    }
    else {
       objets = s.objects;
    }
    int num_objects = objets.size();
    
    for(int i = 0; i < num_objects; i++)
    {
        glPushMatrix();

        {
            int num_transform_sets = objets[i].transform_sets.size();

            for(int j = num_transform_sets - 1; j >= 0; --j)
            {
                glTranslatef(objets[i].transform_sets[j].translation[0],
                             objets[i].transform_sets[j].translation[1],
                             objets[i].transform_sets[j].translation[2]);
                //OpenGL uses degrees, so convert to degrees.
                glRotatef(rad2deg(objets[i].transform_sets[j].rotation_angle),
                          objets[i].transform_sets[j].rotation[0],
                          objets[i].transform_sets[j].rotation[1],
                          objets[i].transform_sets[j].rotation[2]);
                glScalef(objets[i].transform_sets[j].scaling[0],
                         objets[i].transform_sets[j].scaling[1],
                         objets[i].transform_sets[j].scaling[2]);

            }
            glMaterialfv(GL_FRONT, GL_AMBIENT, objets[i].ambient_reflect);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, objets[i].diffuse_reflect);
            glMaterialfv(GL_FRONT, GL_SPECULAR, objets[i].specular_reflect);
            glMaterialf(GL_FRONT, GL_SHININESS, objets[i].shininess);

            glVertexPointer(3, GL_FLOAT, 0, &objets[i].vertex_buffer[0]);
            glNormalPointer(GL_FLOAT, 0, &objets[i].normal_buffer[0]);

            int buffer_size = objets[i].vertex_buffer.size();
            
            if(!wireframe_mode)
                glDrawArrays(GL_TRIANGLES, 0, buffer_size);
            else
                for(int j = 0; j < buffer_size; j += 3)
                    glDrawArrays(GL_LINE_LOOP, j, 3);
        }
        glPopMatrix();
    }
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

/* 'rad2deg' function:
 * 
 * Converts given angle in radians to degrees.
 */

float rad2deg(float angle)
{
    return angle * 180.0 / M_PI;
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
    /* If 's' is pressed, toggle our 'smoothing_mode' boolean to make OpenGL
     * draw the smooth_objects instead of the objects.
     */
    else if(key == 's')
    {
        smoothing_mode = !smoothing_mode;
        glutPostRedisplay();
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
    float h = atof(argv[4]);
    s.h = h;
    s = parse_camera(infile, s.h);
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
