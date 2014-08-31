//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once

#include "../objects/Object.h"
//+------------------------------------------------------------------+
//| Класс для отрисовки и работы c OpenGL
//+------------------------------------------------------------------+
class COpenGL
{
   int  m_width;
   int  m_height;

public:
   COpenGL();
   ~COpenGL();

   SCFRESULT        initialize(const int width, const int height);
   SCFRESULT        resizeWindow(const int width, const int height);
   void             clearWindow();

   static SCFRESULT loadShader(const char * filename, GLenum shader_type, GLuint *shader);
   static SCFRESULT compileShader(const GLuint *shaders, const size_t count, GLuint* program);
   static void      deleteShader(const GLuint *shaders, const size_t count);

   static SCFRESULT getUniforms(ShapeGL &shape, const GLuint * program=NULL);
   static SCFRESULT createShape(const Shape &shape, ShapeGL &shapeGL);
   static SCFRESULT renderShape(ShapeGL &shape);

   static SCFRESULT renderCubeEnvironment(ShapeGL &cube);

   static SCFRESULT attachShadowBuffer(const GLuint shadow_buffer, const GLuint shadow_program);
   void             detachShadowBuffer(); 

};