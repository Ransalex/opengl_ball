#version 410 core

uniform samplerCube tex_cubemap;

in VS_OUT
{
    vec3 text_coord;
} fs_in;

layout (location = 0) out vec4 color;

void main(void)
{
   color = texture(tex_cubemap, fs_in.text_coord);
}
