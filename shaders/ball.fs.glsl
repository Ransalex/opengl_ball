#version 410 core

uniform samplerCube tex_cubemap;
uniform sampler2D   tex_object;

uniform mat4 view_matrix;


in VS_OUT
{  
   vec2 text_coord;
   vec3 view;
   vec3 pos_eye;
   vec3 normal_eye;

   vec3 view_normal;
   vec3 light_vector;
   vec3 view_vector;
} fs_in;

out vec4 outColor;

uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 100.0;
uniform vec3 ambient = vec3(0.1, 0.1, 0.1);
//uniform vec3 rim_color = vec3(0.3, 0.3, 0.3);
uniform float rim_power = 5.0;

vec3 calculate_rim(vec3 N, vec3 V, vec3 rim_color)
{
    float f = 1.0 - dot(N, V);

    f = smoothstep(0.0, 1.0, f);
    f = pow(f, rim_power);

    return f * rim_color;
}

void main(void)
{
   vec3 incident_eye = normalize(fs_in.pos_eye);
   vec3 normal       = normalize(fs_in.normal_eye);
   vec3 reflected    = reflect(incident_eye, fs_in.view_normal);
   vec4 cube_color;
   vec4 ball_color;

   // cube map
   reflected  = vec3(inverse(view_matrix) * vec4(reflected, 0.0));
   cube_color = texture(tex_cubemap, reflected);

   ball_color = texture(tex_object, fs_in.text_coord);

   // phong shading
   vec3 N = normalize(fs_in.view_normal);
   vec3 L = normalize(fs_in.light_vector);
   vec3 V = normalize(fs_in.view_vector);
   vec3 R = reflect(-L, N);

   vec3 diffuse  = max(dot(N, L), 0.0) * vec3(mix(ball_color, cube_color, 0.3));
   vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;

   vec3 amount_color = ambient + diffuse + specular+ball_color.xyz*0.1;

   vec3 rim   = calculate_rim(N, V, amount_color);

   outColor = vec4(amount_color + rim, 1.0);
}
