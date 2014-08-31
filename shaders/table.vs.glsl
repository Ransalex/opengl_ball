#version 420 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 text_coord;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat4 shadow_matrix;
uniform vec3 light_pos;

out view_vectorS_OUT
{
   vec2 text_coord;
   vec4 shadow_coord;
   vec3 view_normal;
   vec3 light_vector;
   vec3 view_vector;
} vs_out;

void main(void)
{
    mat4 mv_matrix = view_matrix * model_matrix;
    vec3 normal    = vec3(0.0, 1.0, 0.0);

    vec4 view_coord = mv_matrix * position;

    vs_out.view_normal  = mat3(mv_matrix) * normal;
    vs_out.light_vector = light_pos - view_coord.xyz;
    vs_out.view_vector  = -view_coord.xyz;
    vs_out.shadow_coord = shadow_matrix * position;
	 vs_out.text_coord   = text_coord;

    gl_Position = proj_matrix * view_coord;
}
