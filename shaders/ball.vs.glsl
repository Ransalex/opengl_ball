 #version 410 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 text_coord;
layout (location = 2) in vec3 normal;
 
uniform mat4 model_matrix;
uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 shadow_matrix;
uniform vec3 light_pos;

out VS_OUT
{
   vec2 text_coord;
   vec3 view;
   vec3 pos_eye;
   vec3 normal_eye;

   vec3 view_normal;
   vec3 light_vector;
   vec3 view_vector;
} vs_out;        

void main(void)
{
  mat4 mv_matrix = view_matrix * model_matrix;
  vec4 pos_vs    = mv_matrix * position;
  
  // cube map
  vs_out.pos_eye    = vec3 (view_matrix * mv_matrix * vec4 (position.xyz, 1.0));
  vs_out.normal_eye = vec3 (view_matrix * mv_matrix * vec4 (normal, 0.0));
  vs_out.text_coord = text_coord;
  vs_out.view       = pos_vs.xyz;

  // phong shading
  vec4 view_coord     = mv_matrix * position;
  vs_out.view_normal  = mat3(mv_matrix) * normal;
  vs_out.light_vector = vec3(view_matrix*vec4(light_pos, 0.0));
  vs_out.view_vector  = -view_coord.xyz;
   
  gl_Position = proj_matrix * pos_vs;
}