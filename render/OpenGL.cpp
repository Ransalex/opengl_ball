//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "OpenGL.h"
//+------------------------------------------------------------------+
//| Конструктор
//+------------------------------------------------------------------+
COpenGL::COpenGL():
   m_width(0),
   m_height(0)
{
}
//+------------------------------------------------------------------+
//| Деструктор
//+------------------------------------------------------------------+
COpenGL::~COpenGL()
{
}
//+------------------------------------------------------------------+
//| Инциализация
//+------------------------------------------------------------------+
SCFRESULT COpenGL::initialize(const int width, const int height)
{
   if(width<=0 || height<=0) ReturnError(RES_E_INVALID_ARGS);
   
   m_width =width;
   m_height=height;

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Изменение размеров окна
//+------------------------------------------------------------------+
SCFRESULT COpenGL::resizeWindow(const int width, const int height)
{
   if(width<=0 || height<=0) ReturnError(RES_E_INVALID_ARGS);
   
   m_width =width;
   m_height=height;

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Очистка окна
//+------------------------------------------------------------------+
void COpenGL::clearWindow()
{
   const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 0.0f };
   const GLfloat one     = 1.0f;

   glCullFace(GL_FRONT_AND_BACK);
   glClearBufferfv(GL_COLOR, 0, white);
   glClearBufferfv(GL_DEPTH, 0, &one);
   glCullFace(GL_FRONT_AND_BACK);
   glEnable(GL_DEPTH_TEST);
   glBindTexture(GL_TEXTURE_2D, 0);

   glViewport(0, 0, m_width, m_height);

   return;
}
//+------------------------------------------------------------------+
//| Загрузка шейдера
//+------------------------------------------------------------------+
SCFRESULT COpenGL::loadShader(const char * filename, GLenum shader_type, GLuint *shader)
{
   if(filename==NULL || shader==NULL) ReturnError(RES_E_INVALID_ARGS);

   GLuint result     =0;
   FILE * shader_file=0;
   size_t filesize   =0;
   GLint  status     =0;
  
   // читаем файл
   if(fopen_s(&shader_file,filename, "rb")!=0 && shader_file==NULL) 
      ReturnError(RES_E_FAIL);

   fseek(shader_file, 0, SEEK_END);
   filesize = ftell(shader_file);

   if(filesize==0)
      ReturnError(RES_E_FAIL);

   fseek(shader_file, 0, SEEK_SET);

   CSmartArray<char> data = new(std::nothrow) char [filesize + 1];
   if(data==NULL)
      ReturnError(RES_E_OUT_OF_MEMORY);

   fread(data, 1, filesize, shader_file);
   data[filesize] = 0;
   fclose(shader_file);

   // компилируем шейдер
   result = glCreateShader(shader_type);
   if(!result)
      ReturnError(RES_E_OPENGL_ERRROR);

   glShaderSource(result, 1, &data, NULL);
   glCompileShader(result);

   glGetShaderiv(result, GL_COMPILE_STATUS, &status);
   if(!status)
   {
      glDeleteShader(result);
      ReturnError(RES_E_OPENGL_ERRROR);
   }

   *shader=result;

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Скопилировать шэйдеры
//+------------------------------------------------------------------+
SCFRESULT COpenGL::compileShader(const GLuint *shaders, const size_t count, GLuint* program)
{
   if((shaders==NULL && count!=0) || program==NULL) ReturnError(RES_E_INVALID_ARGS);

   GLuint result=0;

   *program=0;

   if(count==NULL)
      return(RES_S_OK);

   result = glCreateProgram();
   if(result==0)
      ReturnError(RES_E_OPENGL_ERRROR);

   for(size_t i=0; i< count; ++i)
      glAttachShader(result, shaders[i]);
   
   glLinkProgram(result);

   *program = result;
   
   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Освобождение шэйдера
//+------------------------------------------------------------------+
void COpenGL::deleteShader(const GLuint *shaders, const size_t count)
{
   if(shaders==NULL || count== NULL) return;

   for(size_t i=0; i<count; ++i)
      glDeleteShader(shaders[i]);

   return;
}
//+------------------------------------------------------------------+
//| Заполнение буферов фигуры
//+------------------------------------------------------------------+
SCFRESULT COpenGL::createShape(const Shape &shape, ShapeGL &shapeGL)
{
   GLuint text_buffer  =0;
   GLuint normal_buffer=0;
   GLenum err          =0;
   
   glGenVertexArrays(1, &shapeGL.vao);
   glBindVertexArray(shapeGL.vao);

   if(shape.vertices.size()>0)
   {
      glGenBuffers(1, &shapeGL.vertex_buff);
      glBindBuffer(GL_ARRAY_BUFFER, shapeGL.vertex_buff);
      glBufferData(GL_ARRAY_BUFFER, shape.vertices.size()*sizeof(Point), &shape.vertices[0], GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(0);
   }

   if(shape.text_coord.size()>0)
   {
      glGenBuffers(1, &shapeGL.text_coord_buff);
      glBindBuffer(GL_ARRAY_BUFFER, shapeGL.text_coord_buff);
      glBufferData(GL_ARRAY_BUFFER, shape.text_coord.size()*sizeof(Coord), &shape.text_coord[0], GL_STATIC_DRAW);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(1);
   }

   if(shape.normal.size()>0)
   {
      glGenBuffers(1, &shapeGL.normal_buff);
      glBindBuffer(GL_ARRAY_BUFFER, shapeGL.normal_buff);
      glBufferData(GL_ARRAY_BUFFER, shape.normal.size()*sizeof(Normal), &shape.normal[0], GL_STATIC_DRAW);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(2);
   }
   shapeGL.vertices_size=shape.vertices.size();

   err=glGetError();
   return(err ? RES_E_OPENGL_ERRROR : RES_S_OK);
}
//+------------------------------------------------------------------+
//| Получение смещений для матриц в шейдерах
//+------------------------------------------------------------------+
SCFRESULT COpenGL::getUniforms(ShapeGL &shape, const GLuint *program)
{
   if(shape.program==0 && program==NULL) ReturnError(RES_E_INVALID_ARGS);

   shape.uniform_prog_matrix   =glGetUniformLocation(program!=NULL ? *program : shape.program, "proj_matrix");
   shape.uniform_model_matrix  =glGetUniformLocation(program!=NULL ? *program : shape.program, "model_matrix");
   shape.uniform_view_matrix   =glGetUniformLocation(program!=NULL ? *program : shape.program, "view_matrix");
   shape.uniform_shadow_matrix =glGetUniformLocation(program!=NULL ? *program : shape.program, "shadow_matrix");
   shape.uniform_light_position=glGetUniformLocation(program!=NULL ? *program : shape.program, "light_pos");

   shape.uniform_texture        =glGetUniformLocation(program!=NULL ? *program : shape.program, "tex_object");
   shape.uniform_texture_shadow =glGetUniformLocation(program!=NULL ? *program : shape.program, "tex_shadow");
   shape.uniform_cube_map       =glGetUniformLocation(program!=NULL ? *program : shape.program, "tex_cubemap");

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Отрисовка фигуры
//+------------------------------------------------------------------+
SCFRESULT COpenGL::renderShape(ShapeGL &shape)
{
   GLenum err=0;

   GLuint textres_count=0;
   int    attrib_offset=0;

   if(shape.program>0)
      glUseProgram(shape.program);

   if(shape.cube_map>0 && shape.uniform_cube_map>=0)
   {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, shape.cube_map);
      glUniform1i(shape.uniform_cube_map, 0);

      textres_count++;
   }

   if(shape.texture_shadow>0 && shape.uniform_texture_shadow>=0)
   {
      glActiveTexture(GL_TEXTURE0+textres_count);
      glBindTexture(GL_TEXTURE_2D, shape.texture_shadow);
      glUniform1i(shape.uniform_texture_shadow, textres_count);

      textres_count++;
   }

   if(shape.texture>0 && shape.uniform_texture>=0)
   {
      glActiveTexture(GL_TEXTURE0+textres_count);
      glBindTexture(GL_TEXTURE_2D, shape.texture);
      glUniform1i(shape.uniform_texture, textres_count);

      textres_count++;
   }

   if(shape.model_matrix>=0)
      glUniformMatrix4fv(shape.uniform_model_matrix, 1, GL_FALSE, shape.model_matrix);

   if(shape.uniform_prog_matrix>=0)
      glUniformMatrix4fv(shape.uniform_prog_matrix, 1, GL_FALSE, shape.proj_matrix);

   if(shape.uniform_view_matrix>=0)
      glUniformMatrix4fv(shape.uniform_view_matrix, 1, GL_FALSE, shape.view_matrix);

   if(shape.uniform_shadow_matrix>=0)
      glUniformMatrix4fv(shape.uniform_shadow_matrix, 1, GL_FALSE, shape.shadow_matrix);

  if(shape.uniform_light_position>=0)
      glUniform3fv(shape.uniform_light_position, 1, shape.light_position);

   glBindVertexArray(shape.vao);
   glBindBuffer(GL_ARRAY_BUFFER,shape.vertex_buff);
   glVertexAttribPointer(attrib_offset, 3, GL_FLOAT, GL_FALSE, 0, NULL);
   glDrawArrays(GL_TRIANGLES, 0,  shape.vertices_size );

   glBindTexture(GL_TEXTURE_2D, 0);

   err=glGetError();
   return(err ? RES_E_OPENGL_ERRROR : RES_S_OK);
}
//+------------------------------------------------------------------+
//| Отрисовка окружения
//+------------------------------------------------------------------+
 SCFRESULT COpenGL::renderCubeEnvironment(ShapeGL &cube)
 {
    GLenum err=0;

    glBindVertexArray(cube.vao);

    glUseProgram(cube.program);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube.cube_map);    
    glUniformMatrix4fv(cube.uniform_view_matrix, 1, GL_FALSE, cube.view_matrix);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_DEPTH_TEST);

    err=glGetError();
    return(err ? RES_E_OPENGL_ERRROR : RES_S_OK);
 }
 //+------------------------------------------------------------------+
 //| Биндинг буфера для тени
 //+------------------------------------------------------------------+
 SCFRESULT COpenGL::attachShadowBuffer(const GLuint shadow_buffer, const GLuint shadow_program)
 {
    if(shadow_buffer==0) ReturnError(RES_E_INVALID_ARGS);

    GLenum       err     =0;
    const GLenum buffs[] ={ GL_COLOR_ATTACHMENT0 };
    const GLfloat zero[] ={ 0.0f };
    const GLfloat ones[] ={ 1.0f };

    glUseProgram(shadow_program);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 4.0f);    
    glDrawBuffers(1, buffs);
    glClearBufferfv(GL_COLOR, 0, zero);
    glClearBufferfv(GL_DEPTH, 0, ones);

    err=glGetError();
    return(err ? RES_E_OPENGL_ERRROR : RES_S_OK);
 }
 //+------------------------------------------------------------------+
 //| ОТкрепляем буфер для теней
 //+------------------------------------------------------------------+
 void COpenGL::detachShadowBuffer()
 {
    static const GLfloat gray[] = { 1.0f, 1.0f, 1.0f, 0.0f };

    glDisable(GL_POLYGON_OFFSET_FILL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
    glClearBufferfv(GL_COLOR, 0, gray);
 }