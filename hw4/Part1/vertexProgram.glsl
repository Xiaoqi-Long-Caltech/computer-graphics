
varying vec3 normal, world_vec;
uniform int nlights;
uniform vec3 cam;
void main()
{
    vec3 world = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z);
    world_vec = vec3(gl_ModelViewMatrix * gl_Vertex);
    normal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(world, 1.0);
    gl_FrontColor = vec4(1, 0, 0, 1);
}
