
attribute vec3 tangent;
uniform vec3 cam;
uniform float size, toggle;
varying float u, v;
varying vec3 light_vec, world_vec, cam_vec;

void main()
{
    u = gl_Vertex.x;
    v = gl_Vertex.y;
    
    float z = 0.0;
    
    // set vector with the vertex
    vec3 world = vec3(u, v, z);
    // transform points using the model view matrix
    world_vec = vec3(gl_ModelViewMatrix * vec4(world, 1.0));
    gl_Position = gl_ModelViewProjectionMatrix * vec4(world, 1.0);
    
    vec3 t = normalize(gl_NormalMatrix * tangent); //tangent, +u direction
    vec3 n = normalize(gl_NormalMatrix * gl_Normal); //normal vector
    vec3 b = normalize(cross(n, t)); //bitangent, +v direction
    
    //camera direction in camera space
    vec3 cam_dir = normalize(cam - world_vec);
    //light direction in camera space
    vec3 cam_light_dir = normalize((vec3(gl_LightSource[0].position) - world_vec));
    
    //camera direction in surface space 
    //s = TBN * c
    cam_vec = normalize(vec3(dot(t,cam_dir), dot(b,cam_dir), dot(n,cam_dir)));
    //light direction in surface space
    light_vec = normalize(vec3(dot(t,cam_light_dir), dot(b,cam_light_dir), dot(n,cam_light_dir)));
    
    float tu = (u + (size / 2.0)) / size;
    float tv = (v + (size / 2.0)) / size;
    gl_TexCoord[0] = vec4(tu, tv, 0.0, 1.0);
    gl_TexCoord[1] = vec4(tu, tv, 0.0, 1.0);
    
}
