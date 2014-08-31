#version 420 core

uniform sampler2DShadow tex_shadow;
uniform sampler2D       tex_object;

// свойства материала
uniform vec3 diffuse_albedo  = vec3(0.0, 0.5, 0.0);
uniform vec3 specular_albedo = vec3(0.0, 0.7, 1.0);
uniform float specular_power = 100.0;

in view_vectorS_OUT
{
   vec2 text_coord;
   vec4 shadow_coord;
   vec3 view_normal;
   vec3 light_vector;
   vec3 view_vector;
} fs_in;

layout (location = 0) out vec4 color;

float percentageCloserFilter(in vec4 smcoord)
{
   float res = 0.0;

   res += textureProjOffset(tex_shadow, smcoord, ivec2(-1,-1));
   res += textureProjOffset(tex_shadow, smcoord, ivec2( 0,-1));
   res += textureProjOffset(tex_shadow, smcoord, ivec2( 1,-1));
   res += textureProjOffset(tex_shadow, smcoord, ivec2(-1, 0));
   res += textureProjOffset(tex_shadow, smcoord, ivec2( 0, 0));
   res += textureProjOffset(tex_shadow, smcoord, ivec2( 1, 0));
   res += textureProjOffset(tex_shadow, smcoord, ivec2(-1, 1));
   res += textureProjOffset(tex_shadow, smcoord, ivec2( 0, 1));
   res += textureProjOffset(tex_shadow, smcoord, ivec2( 1, 1));

   return (res / 9.0);
}

void main(void)
{
   vec3 view_normal  = normalize(fs_in.view_normal);
   vec3 light_vector = normalize(fs_in.light_vector);
   vec3 view_vector  = normalize(fs_in.view_vector);
   vec3 reflect      = reflect(-light_vector, view_normal);
   vec4 shadow_color;
   vec4 table_color;

   vec3 diffuse  = max(dot(view_normal, light_vector), 0.0) * diffuse_albedo;
   vec3 specular = pow(max(dot(reflect, view_vector), 0.0), specular_power) * specular_albedo;

   float shadow = percentageCloserFilter(fs_in.shadow_coord);
   shadow_color = shadow * mix(vec4(1.0), vec4(diffuse + specular, 1.0), bvec4(1));

   table_color  = texture(tex_object, fs_in.text_coord);

   color = mix(table_color, shadow_color, 0.7);
}
