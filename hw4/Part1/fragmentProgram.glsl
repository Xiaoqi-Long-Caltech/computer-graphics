
varying vec3 normal, world_vec;
uniform int nlights;
uniform vec3 cam;
void main()
{
        vec3 c_diffuse = vec3(gl_FrontMaterial.diffuse); 
        vec3 c_ambient = vec3(gl_FrontMaterial.ambient); 
        vec3 c_specular = vec3(gl_FrontMaterial.specular);
        float shine = gl_FrontMaterial.shininess;
        vec3 diffuse_sum = vec3(0.0);
        vec3 specular_sum = vec3(0.0);        
        vec3 norm = normalize(normal);
        vec3 cam_vec = normalize(cam - world_vec);
        for (int i = 0; i < nlights; i++) {
            //attenuation
            vec3 l_position = vec3(gl_LightSource[i].position) - world_vec;
            float l_distance = length(l_position);
            float k = gl_LightSource[i].quadraticAttenuation;
            float atten = 1.0 / (1.0 + k * l_distance * l_distance);
            vec3 c_light = vec3(gl_LightSource[i].ambient);
            c_light *= atten;
            vec3 l_direction = normalize(l_position);
            //diffuse
            vec3 l_diffuse = c_light * max(0.0, dot(norm, l_direction));
            diffuse_sum += l_diffuse;
            //specular
            float specular = max(0.0, dot(norm, normalize(cam_vec + l_direction)));
            vec3 spec_light = c_light * pow(specular, shine);
            specular_sum += spec_light;
       }
       vec3 prod_diff = vec3(c_diffuse.r * diffuse_sum.r, c_diffuse.g * diffuse_sum.g, c_diffuse.b * diffuse_sum.b);
       vec3 prod_spec = vec3(c_specular.r * specular_sum.r, c_specular.g * specular_sum.g, c_specular.b * specular_sum.b);
       vec3 color = c_ambient + prod_diff + prod_spec;
       gl_FragColor = vec4(min(1.0, color.r), min(1.0, color.g), min(1.0, color.b), 1);
}
