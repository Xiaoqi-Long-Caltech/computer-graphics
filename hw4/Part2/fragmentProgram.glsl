
uniform float size, toggle;
varying float u, v;
uniform vec3 cam;
varying vec3 light_vec, world_vec, cam_vec;
uniform sampler2D colorTex, normalTex;

void main()
{    
    vec4 texcoord0 = gl_TexCoord[0];
    vec4 texcoord1 = gl_TexCoord[1];
    
    //as instructed
    vec4 colorVec = texture2D(colorTex, texcoord0.st);
    vec3 normalVec = (texture2D(normalTex, texcoord1.st)).rgb;
    float r = normalVec.r;
    float g = normalVec.g;
    float b = normalVec.b;
    //Map components [0, 1] -> [-1, 1] here
    vec3 mapped = vec3((r * 2.0) - 1.0, (g * 2.0) - 1.0, (b * 2.0) - 1.0);
    //renormalize normal vectors
    vec3 normal = normalize(mapped);
    
    //diffuse
    vec3 c_diffuse = colorVec.rgb;
    //ambient
    vec3 c_ambient = vec3(gl_FrontMaterial.ambient); 
    //specular
    vec3 c_specular = vec3(gl_FrontMaterial.specular);
    
    vec3 cam_light = vec3(gl_LightSource[0].position) - world_vec;
    float len = length(cam_light);
    float k = gl_LightSource[0].quadraticAttenuation;
    float atten = 1.0 / (1.0 + k * len * len);
    vec3 c_light = vec3(gl_LightSource[0].diffuse);
    c_light *= atten;
    vec3 diff_light = c_light * max(0.0, dot(normal, light_vec));
    float specular = max(0.0, dot(normal, normalize(cam_vec + light_vec)));
    vec3 spec_light = c_light * pow(specular, 5.0);
    
    vec3 prod_diff = vec3(c_diffuse.r * diff_light.r, c_diffuse.g * diff_light.g, c_diffuse.b * diff_light.b);
    vec3 prod_spec = vec3(c_specular.r * spec_light.r, c_specular.g * spec_light.g, c_specular.b * spec_light.b);
    vec3 color = c_ambient + prod_diff + prod_spec;
    gl_FragColor = vec4(min(1.0, color.r), min(1.0, color.g), min(1.0, color.b), 1);
    
}
