//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once

#include "OpenGL.h"

#include "../objects/ShapeGenerator.h"
#include "TextureLoader.h"
//+------------------------------------------------------------------+
//| Класс сцены
//+------------------------------------------------------------------+
class CScene
{
private:
   int        m_width;
   int        m_height;

   ShapeGL    m_cube;
   ShapeGL    m_ball;
   ShapeGL    m_plane;

   ShapeGL    m_ball_shadow;
   ShapeGL    m_plane_shadow;

   GLuint     m_shadow_program;
   GLuint     m_shadow_frame_buffer;
   GLuint     m_shadow_texture;
   
   math::vec3 m_view_position;
   math::mat4 m_view_matrix;
   math::mat4 m_proj_matrix;
   math::mat4 m_shadow_sbpv_matrix;

   math::vec3 m_light_position;
   math::mat4 m_light_view_matrix;
   math::mat4 m_light_proj_matrix;

   COpenGL         m_render;
   CTextureLoader  m_texture_loader;
   CShapeGenerator m_shape_generator;

   double     m_last_time;
   double     m_ball_speed;
   bool       m_ball_fall;
   bool       m_ball_selected;

public:
      CScene();
      ~CScene();

   void      setViewPointer();
   
   SCFRESULT initialize(const int width, const int height);
   void      update(double currentTime);
   SCFRESULT render(); 

   void      resizeWindow(int width, int height);
   void      moveCamera(int distance);
   void      selectObject(int mouse_x, int mouse_y);
   void      unselectObject();
   void      moveObject(int distance_x, int distance_y);

private:
   GLuint    compile_shaders(const char* vertex_shader, const char* fragment_shader) const;

   void      initiailizeMatrix();
   void      clearBuffers(ShapeGL &shape);
   void      recalculateViewMatix();
   bool      directionUnchange(double last_speed, double curr_speed);

};