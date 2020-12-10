#include "object.h"
#include "scene.h"

#include <iostream>

#include "image.h"

using namespace Eigen;
using namespace std;

const int MAX_ITERS = 10000;
const int XRES = 500;
const int YRES = 500;

/**
 * IOTest Code
 */

bool Superquadric::IOTest(const Vector3d &point) {
    /**
     * PART 1
     *       Implement the IO Test function for a superquadric. Remember to
     *       apply any transformations to the superquadric before testing
     *       against the IO function.
     */
    //store transformations
    Matrix4d transformations = Matrix4d::Identity();
    // calculates the inverse transformation matrix O^-1
    for (auto& trans : transforms) {
       Matrix4d m = trans->GetMatrix();
       transformations = m * transformations;
    }
    // transform the point
    Matrix4d inv_transformations = transformations.inverse();
    
    Vector4d n_point;
    n_point << point(0), point(1), point(2), 1.0;
    n_point = inv_transformations * n_point;
    double x, y, z;
    x = n_point(0);
    y = n_point(1);
    z = n_point(2);
    double s = -1 + pow(z * z, 1.0/exp1) + pow(pow(x * x, 1.0 / exp0) + pow(y * y, 1.0 / exp1) , exp0/exp1);
    if (s > 0) {
       return false;
    }
    return true;
}

bool Assembly::IOTest(const Vector3d &point) {
    Matrix4d transformations = Matrix4d::Identity();
    // calculates the inverse transformation matrix O^-1
    for (auto& trans : transforms) {
       Matrix4d m = trans->GetMatrix();
       transformations = m * transformations;
    }
    Matrix4d inv_transformations = transformations.inverse();
    // transform the point
    Vector4d n_point;
    n_point << point(0), point(1), point(2), 1.0;
    n_point = inv_transformations * n_point;
    double x, y, z;
    x = n_point(0);
    y = n_point(1);
    z = n_point(2);
    Vector3d p_point = {x, y, z};
    bool IO_result = false;
    // recurse on children
    for (auto& child : children) {
       IO_result |= child->IOTest(p_point);
    }
    return IO_result;
}

/**
 * Closest Intersection Code
 */
 
// Some helper functions
// This is the inside-outside function for a Superquadratic.
double SFunc(Vector3d point, float e, float n) {
    double x = point(0);
    double y = point(1);
    double z = point(2);
    double s = -1 + pow(z * z, 1.0/ n) + pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e) , e / n);
    return s;
}

// This is the gradient of the above function
Vector3d SGrad(Vector3d point, float e, float n) {
    double x = point(0);
    double y = point(1);
    double z = point(2);
    double Gx = (x == 0.0) ? 0.0 : (2 * x * pow(x * x, 1.0 / e - 1.0) * pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e), e / n - 1.0));
    double Gy = (y == 0.0) ? 0.0 : (2 * y * pow(y * y, 1.0 / e - 1.0) * pow(pow(y * y, 1.0 / e) + pow(x * x, 1.0 / e), e / n - 1.0));
    double Gz = (z == 0.0) ? 0.0 : 2 * z * pow(z * z, 1.0 / n - 1.0);
    Vector3d G((1.0 / n) * Gx, (1.0 / n) * Gy, (1.0 / n) * Gz);
    return G;
}

// g(t) = S(ray(t))
double g_t(Ray ray, double t, double e, double n) {
    Vector3d point = ray.At(t);
    return SFunc(point, e, n);
}

// g'(t) = a dot Grad(S(ray(t)))
double g_d(Ray ray, double t, double e, double n) {
    Vector3d point = ray.At(t);
    Vector3d grad;
    grad = SGrad(point, e, n);
    double res = ray.direction.dot(grad);
    return res;
}

pair<double, Intersection> Superquadric::ClosestIntersection(const Ray &ray) {
    /**
     * PART 1
     *       Implement a ray-superquadric intersection using Newton's method.
     *       Make sure to apply any transformations to the superquadric before
     *       performing Newton's method.
     */
    pair<double, Intersection> closest = make_pair(INFINITY, Intersection());
    // First, get the transformation matrix.
    Matrix4d transformations = Matrix4d::Identity();
    // calculates the inverse transformation matrix O^-1
    for (auto& trans : transforms) {
       Matrix4d m = trans->GetMatrix();
       transformations = m * transformations;
    }
    Matrix4d inv_transformations = transformations.inverse();
    // transform the ray
    Ray new_ray = ray.Transformed(inv_transformations);
    Vector3d a_vec = new_ray.direction;
    Vector3d b_vec = new_ray.origin;
    
    // Calculate a, b, c; check the discriminant delta
    double a = a_vec.dot(a_vec);
    double b = 2.0 * (a_vec.dot(b_vec));
    double c = b_vec.dot(b_vec) - 3.0;
    double delta = b * b - 4 * a * c;
    // If delta < 0, then the ray does not intersect with the bounding sphere
    if (delta < 0) {
       return closest;
    }
    
    double t;
    if (b >= 0) {
       t = (- b - sqrt(delta)) / (2 * a);
    }
    else {
       t = 2 * c / (- b + sqrt(delta));
    }
    
    // Delta >= 0, then proceed to use Newton's method
    // initial guess should be t-
    if (t < 0) {
       // superquadratic can't be seen
       return closest;
    }
    
    // using t- when it is positive should be sufficient
    // the following is Newton's method
    while (true) {
       double g = g_t(new_ray, t, exp0, exp1);
       double g_p = g_d(new_ray, t, exp0, exp1);
       if (abs(g) < 0.001) {
          // the origin is the position where the intersection occurs in parent space, and the direction is the normal vector of the Superquadric at that point in parent space
          Ray location;
          location.origin = new_ray.At(t);
          location.direction = GetNormal(new_ray.At(t));
          location.Transform(transformations);
          location.Normalize();
          // make a pair of the time value of the intersection happened, and the intersection
          pair<double, Intersection> res = make_pair(t, Intersection(location, this));
          return res;
       }
       if (g_p >= 0) {
          // we've missed the superquadratic
          return closest;
       }
       t = t - (g / g_p);
    }
    return closest;
    
}

pair<double, Intersection> Assembly::ClosestIntersection(const Ray &ray) {
    /**
     * PART 1
     *       Implement a ray-assembly intersection by recursively finding
     *       intersection with the assembly's children. Make sure to apply any
     *       transformations to the assembly before calling ClosestIntersection
     *       on the children.
     */
    // First transform the assembly
    Matrix4d transformations = Matrix4d::Identity();
    for (auto& trans : transforms) {
       Matrix4d m = trans->GetMatrix();
       transformations = m * transformations;
    }
    Matrix4d inv_transformations = transformations.inverse();
    Ray new_ray = ray.Transformed(inv_transformations);
    // set initial pair as the pair at infinity
    pair<double, Intersection> closest = make_pair(INFINITY, Intersection());
    // recurse over children
    for (auto& child : children) {
        pair<double, Intersection> child_intersect = child->ClosestIntersection(new_ray);
        // update the closest intersection
        if (child_intersect.first < closest.first) {
           closest = child_intersect;
        }
    }
    Ray location = closest.second.location;
    location.Transform(transformations);
    location.Normalize();
    Intersection in(location, closest.second.obj);
    pair<double, Intersection> res = make_pair(closest.first, in);
    return res;
}

/**
 * Raytracing Code
 */
 
Vector3f Scene::Phong(const Vector3d &point, const Vector3d &normal, const Material &material, const vector<Light> &lights, const Vector3d &cam) const {
    Vector3f diffuse_sum(0, 0, 0);
    Vector3f specular_sum(0, 0, 0);
    Vector3f v_diffuse = material.diffuse.ToVector();
    Vector3f v_specular = material.specular.ToVector();
    Vector3f v_ambient = material.ambient.ToVector();
    float shine = material.shininess;
    Vector3d cam_vec = (cam - point).normalized();
    
    for (const Light &l : lights) {
       Vector3d l_position(l.position.x() / l.position.w(), 
                           l.position.y() / l.position.w(),
                           l.position.z() / l.position.w());
       // declare a ray from light to the object
       Ray ray;
       ray.origin = l_position;
       ray.direction = point - l_position;
       double time = ClosestIntersection(ray).first;
       // as instructed in the lecture notes, when t = 1 we've reached the intersection point, so move on
       if (time < 1.0 - 0.001) continue;

       float l_distance = ray.direction.norm();
       float k = l.attenuation;
       float atten = 1.0 / (1.0 + k * l_distance);
       Vector3f c_light = l.color.ToVector();
       c_light *= atten;
       
       Vector3d l_direction = (l_position - point).normalized();
       //diffuse
       Vector3f l_diffuse = c_light * max((double)0.0, normal.dot(l_direction));
       diffuse_sum += l_diffuse;
       //specular
       Vector3f spec_light = c_light * pow(max((double)0.0, normal.dot((cam_vec + l_direction).normalized())), shine);
       specular_sum += spec_light;
    }
    
    Vector3f prod_diff = diffuse_sum.cwiseProduct(v_diffuse);
    Vector3f prod_spec = specular_sum.cwiseProduct(v_specular);
    Vector3f color = v_ambient + prod_diff + prod_spec;
    Vector3f res(min((float)1.0, color(0)), min((float)1.0, color(1)), min((float)1.0, color(2)));
    
    return res;
}

void Scene::Raytrace() {
    Image img = Image(XRES, YRES);
    
    const Camera camera = GetCamera();
    const Frustum frustum = camera.frustum;
    
    const double fov = frustum.fov * M_PI / 180.0;
    const double h = 2 * frustum.near * tan(fov / 2);
    const double w = frustum.aspect_ratio * h;
    
    // compute the transformation matrix out of the loop
    const Matrix4d transformation = (camera.translate.GetMatrix() * camera.rotate.GetMatrix()).inverse();
    
    for (int i = 0; i < XRES; i++) {
        for (int j = 0; j < YRES; j++) {
            Ray ray;
            ray.origin = Vector3d(0, 0, 0);
            Vector3d x_i = (i - XRES / 2) * w / XRES * Vector3d(1, 0, 0);
            Vector3d y_j = (j - YRES / 2) * h / YRES * Vector3d(0, 1, 0);
            // n * e1 + x_i * e2 + y_j * e3
            ray.direction = frustum.near * Vector3d(0, 0, -1) + x_i + y_j;
            ray.Transform(transformation);
            
            Intersection intersection = ClosestIntersection(ray).second;
            if (intersection.obj) {
               // if there is an intersection
               Vector3d point = intersection.location.origin;
               Vector3d normal = intersection.location.direction;
               Material material = intersection.obj->GetMaterial();
               vector<Light> lights = GetLights();
               Vector3d cam = ray.origin;
               Vector3f color = Phong(point, normal, material, lights, cam);
               img.SetPixel(i, j, color);
            }
            else {
               // else, set to black
               img.SetPixel(i, j, Vector3f::Zero());
            }            
        }
    }

    // Outputs the image.
    if (!img.SaveImage("rt.png")) {
        cerr << "Error: couldn't save PNG image" << std::endl;
    } else {
        cout << "Done!\n";
    }
}
