#version 410 core 

layout (location = 0) in vec4 position; 

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main(void)
{
   mat4 mv_matrix = view_matrix * model_matrix;

   gl_Position = proj_matrix * mv_matrix * position;
}
