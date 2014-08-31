//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "Scene.h"
//+------------------------------------------------------------------+
//| Конструктор
//+------------------------------------------------------------------+
CScene::CScene():
   m_width(0),
   m_height(0),
   m_shadow_program(0),
   m_shadow_frame_buffer(0),
   m_shadow_texture(0),
   m_ball_selected(false),
   m_ball_speed(0),
   m_ball_fall(true)
{
   ZeroMemory(&m_cube,  sizeof(m_cube));
   ZeroMemory(&m_ball,  sizeof(m_ball));
   ZeroMemory(&m_plane, sizeof(m_plane));
}
//+------------------------------------------------------------------+
//| Деструктор
//+------------------------------------------------------------------+
CScene::~CScene()
{
   if(m_ball.program)
      glDeleteProgram(m_ball.program);

   if(m_cube.program)
      glDeleteProgram(m_cube.program);

   if(m_plane.program)
      glDeleteProgram(m_plane.program);

   clearBuffers(m_ball);
   clearBuffers(m_plane);

    if(m_ball.texture)
       glDeleteTextures(1, &m_ball.texture);

    if(m_cube.cube_map)
       glDeleteTextures(1, &m_cube.cube_map);

   if(m_shadow_texture)
      glDeleteTextures(1, &m_shadow_texture);

    glfwTerminate();
}
//+------------------------------------------------------------------+
//| Очистка буферов
//+------------------------------------------------------------------+
void CScene::clearBuffers(ShapeGL &shape)
{
   if(shape.vertex_buff)
      glDeleteBuffers(1, &shape.vertex_buff);

   if(shape.text_coord_buff)
      glDeleteBuffers(1, &shape.text_coord_buff);

   if(shape.normal_buff)
      glDeleteBuffers(1, &shape.normal_buff);
}
//+------------------------------------------------------------------+
//| Скомпилировать вершинный и пиксельный шэйдеры
//+------------------------------------------------------------------+
GLuint CScene::compile_shaders(const char* vertex_shader, const char* fragment_shader) const
{
   SCFRESULT res            =RES_S_OK;
   GLuint    ball_shaders[2]={0};
   GLuint    program        =0;

   if(RES_FAIL(res=m_render.loadShader(vertex_shader, GL_VERTEX_SHADER, &ball_shaders[0])))
      ReturnError(res);

   if(RES_FAIL(m_render.loadShader(fragment_shader, GL_FRAGMENT_SHADER, &ball_shaders[1])))
      ReturnError(res);

   if(RES_FAIL(res=m_render.compileShader(ball_shaders, _countof(ball_shaders), &program)))
      ReturnError(res);

   m_render.deleteShader(ball_shaders, _countof(ball_shaders));

   return(program);
}
//+------------------------------------------------------------------+
//| Построение сцены
//+------------------------------------------------------------------+
SCFRESULT CScene::initialize(const int width, const int height)
{
   SCFRESULT res=RES_S_OK;
   Shape     cube;
   Shape     ball;
   Shape     plane;

   m_width =width;
   m_height=height;

   m_render.initialize(width, height);
   initiailizeMatrix();

   // инициализируем окружение
   m_cube.program=compile_shaders("shaders\\skybox.vs.glsl", "shaders\\skybox.fs.glsl");

   if(RES_FAIL(res=m_render.createShape(cube, m_cube)))
      ReturnError(res);

   if(RES_FAIL(res=m_texture_loader.loadTextureCubic("textures\\mountaincube.ktx", &m_cube.cube_map)))
      ReturnError(res);

   if(RES_FAIL(res=m_render.getUniforms(m_cube)))
      ReturnError(res);

   m_cube.view_matrix =m_view_matrix;

    // инициализируем шар
   if(RES_FAIL(res=m_shape_generator.createSphere(1, 40, 40, ball)))
      ReturnError(res);

   if(RES_FAIL(res=m_render.createShape(ball, m_ball)))
      ReturnError(res);

   m_ball.program        =compile_shaders("shaders\\ball.vs.glsl", "shaders\\ball.fs.glsl");
   m_ball.model_matrix   =math::rotate(180.0f, 1.0f, 0.0f, 0.0f) *math::translate(0.0f, 2.0f, 0.0f);
   m_ball.proj_matrix    =m_proj_matrix;
   m_ball.view_matrix    =m_view_matrix;
   m_ball.light_position=m_light_position;
   m_ball.cube_map       =m_cube.cube_map;
   
   if(RES_FAIL(res=m_texture_loader.loadTexturePng("textures\\ball_albedo.png", &m_ball.texture)))
      ReturnError(res);   

   if(RES_FAIL(res=m_render.getUniforms(m_ball)))
      ReturnError(res);

    math::mat4 df=math::translate(0.0f, 2.0f, 0.0f);
    math::vec4 ss=math::vec4(0.0f, 0.0f, 0.0f, 1.0f)*((m_proj_matrix*m_view_matrix*m_ball.model_matrix).transpose());
    DebugOutput("x=%d, y=%d\n", ss[0], ss[1]);
   // инициализируем плоскость
   if(RES_FAIL(res=m_shape_generator.createSquarePlane(10, plane)))
      ReturnError(res);

   if(RES_FAIL(res=m_render.createShape(plane, m_plane)))
      ReturnError(res);

   m_plane.program       =compile_shaders("shaders\\table.vs.glsl", "shaders\\table.fs.glsl");
   m_plane.model_matrix  =math::translate(0.0f, -3.0f, 0.0f);
   m_plane.proj_matrix   =m_proj_matrix;
   m_plane.view_matrix   =m_view_matrix;
   m_plane.light_position=m_light_position;
   m_plane.shadow_matrix =m_shadow_sbpv_matrix * m_plane.model_matrix;   

   if(RES_FAIL(res=m_texture_loader.loadTexturePng("textures\\table.png", &m_plane.texture)))
      ReturnError(res);   

   if(RES_FAIL(res=m_render.getUniforms(m_plane)))
      ReturnError(res);

   m_shadow_program=compile_shaders("shaders\\light.vs.glsl", "shaders\\light.fs.glsl");
 
   if(RES_FAIL(res=m_texture_loader.generateShadowTexture(&m_shadow_frame_buffer, &m_shadow_texture)))
      ReturnError(res);

   m_plane.texture_shadow=m_shadow_texture;
   m_ball.texture_shadow =m_shadow_texture;

   // инициализируем объекты для карты теней
   m_ball_shadow =m_ball;
   m_plane_shadow=m_plane;

   m_ball_shadow.program =0;
   m_ball_shadow.cube_map=0;
   m_ball_shadow.texture =0;
   m_plane_shadow.program=0;

   m_ball_shadow.proj_matrix =m_light_proj_matrix;
   m_ball_shadow.view_matrix =m_light_view_matrix;
   m_plane_shadow.proj_matrix=m_light_proj_matrix;
   m_plane_shadow.view_matrix=m_light_view_matrix;

   m_render.getUniforms(m_ball_shadow, &m_shadow_program);
   m_render.getUniforms(m_plane_shadow, &m_shadow_program);

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Инициализация матриц
//+------------------------------------------------------------------+
void CScene::initiailizeMatrix()
{
   const math::mat4 scale_bias_matrix = math::mat4(math::vec4(0.5f, 0.0f, 0.0f, 0.0f),
                                                   math::vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                                   math::vec4(0.0f, 0.0f, 0.5f, 0.0f),
                                                   math::vec4(0.5f, 0.5f, 0.5f, 1.0f));
   math::mat4 light_vp_matrix;   
   
   m_view_position=math::vec3(0.0f, 0.0f, -10.0f);  // TODO вынести

   recalculateViewMatix();   

   m_light_position    =math::vec3(-10.0f, 20.0f, -20.0f); // TODO вынести
   m_light_proj_matrix =math::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
   m_light_view_matrix =math::lookat(m_light_position , math::vec3(0.0f), math::vec3(0.0f, 1.0f, 0.0f));

   light_vp_matrix      =m_light_proj_matrix * m_light_view_matrix;
   m_shadow_sbpv_matrix =scale_bias_matrix * m_light_proj_matrix * m_light_view_matrix;
}
//+------------------------------------------------------------------+
//| Пересчет матриц для камеры
//+------------------------------------------------------------------+
void CScene::recalculateViewMatix()
{
   m_proj_matrix  =math::perspective(40.0f, (float)m_width/(float)m_height, 0.1f, 1000.0f);
   m_view_matrix  =math::lookat(m_view_position, math::vec3(0.0f, 0.0f, 0.0f), math::vec3(0.0f, 1.0f, 0.0f));

   m_cube.view_matrix =m_view_matrix;
   m_ball.proj_matrix =m_proj_matrix;
   m_ball.view_matrix =m_view_matrix;
   m_plane.proj_matrix=m_proj_matrix;
   m_plane.view_matrix=m_view_matrix;
}
//+------------------------------------------------------------------+
//|  Перемещение камеры вокруг оси Y
//+------------------------------------------------------------------+
void CScene::moveCamera(int distance)
{
   float old_x   =m_view_position[0];
   float old_z   =m_view_position[2];
   float radiance=math::radians<float>((float)distance)/2.0f;

   m_view_position[0]=old_x*cosf(radiance)-old_z*sinf(radiance);
   m_view_position[2]=old_x*sinf(radiance)+old_z*cosf(radiance);

   recalculateViewMatix();   
}
//+------------------------------------------------------------------+
//| Изменение размеров окна
//+------------------------------------------------------------------+
void CScene::resizeWindow(int width, int height)
{
   if(width<=0 || height<=0) return;

   float aspect =(float)width / (float)height;

   m_width =width;
   m_height=height;

   m_proj_matrix=math::perspective(40.0f, aspect, 0.1f, 100.0f);

   m_render.resizeWindow(width, height);

   recalculateViewMatix();

   return;
}
//+------------------------------------------------------------------+
//| Клик по объекту
//| TODO надо делать ray tracing на сферу
//+------------------------------------------------------------------+
void CScene::selectObject(int mouse_x, int mouse_y)
{
   if(mouse_x<0 || mouse_y<0) return;

   m_ball_selected=true;
 
   return;
}
//+------------------------------------------------------------------+
//| Клик по объекту
//+------------------------------------------------------------------+
void CScene::unselectObject()
{
   m_ball_selected=false;

   return;
}
//+------------------------------------------------------------------+
//| Перемещение шара
//+------------------------------------------------------------------+
void CScene::moveObject(int distance_x, int distance_y)
{
    math::vec4 ball_position=math::vec4(0.0f, 0.0f, 0.0f, 1.0f)* m_ball.model_matrix.transpose();

   if(ball_position[1]<=-2 && distance_y>0)
      return;

   float plane_pos=-2.0f;
   float move_y   =(float)distance_y/100;

   if(ball_position[1]-move_y<plane_pos)
      move_y=(ball_position[1]-plane_pos);

   m_ball.model_matrix=m_ball.model_matrix*math::translate(0.0f, move_y, 0.0f);

   m_ball_shadow.model_matrix=m_ball.model_matrix;

   m_ball_speed=0;
   m_ball_fall =true;

   return;
}
//+------------------------------------------------------------------+
//| Обновление сцены
//+------------------------------------------------------------------+
void CScene::update(double currentTime)
{
   double     ball_move      =0;
   double     time_duration  =currentTime-m_last_time;
   math::vec4 ball_position  =math::vec4(0.0f, 0.0f, 0.0f, 1.0f)* m_ball.model_matrix.transpose();
   double     ball_pos_y     =ball_position[1];
   double     plane_pos_y    =-2;
   double     ball_last_speed=m_ball_speed;

   if(!m_ball_selected)
   {
      if(ball_pos_y<=plane_pos_y)
      {
         if(abs(m_ball_speed)<=0.5)
            m_ball_speed=0;

         if(m_ball_speed!=0)
         {
            if(m_ball_fall)
            {
               // отскок шара
               m_ball_speed  = -m_ball_speed*0.5;
               ball_move     = m_ball_speed*time_duration;
               m_ball_fall= false;
            }
            else
            {
               // изменение скорости шара
               m_ball_speed +=9.8f*time_duration;
               ball_move     =m_ball_speed*time_duration;
               if(directionUnchange(ball_last_speed,m_ball_speed))
                  m_ball_fall=true;
            }            
         }
         else
         {
            ball_move=(ball_pos_y-plane_pos_y);
         }
      }
      else
      {
         // изменение скорости шара
         m_ball_speed +=9.8f*time_duration;
         ball_move     =m_ball_speed*time_duration;
         if(directionUnchange(ball_last_speed,m_ball_speed))
            m_ball_fall=true;
      }  

      if(ball_move!=0)
      {
         m_ball.model_matrix       =m_ball.model_matrix*math::translate(0.0f, (float)ball_move, 0.0f);
         m_ball_shadow.model_matrix=m_ball.model_matrix;
      }
      
   }      

   m_last_time=currentTime;
   return;
}
//+------------------------------------------------------------------+
//| проверка изменилось направление движения
//+------------------------------------------------------------------+
bool CScene::directionUnchange(double last_speed, double curr_speed)
{
   // проверяем знаковый бит
   return(last_speed<0 && curr_speed>0);
}
//+------------------------------------------------------------------+
//| Отрисовка сцены
//+------------------------------------------------------------------+
SCFRESULT CScene::render()
{
   GLenum err=0;

   m_render.clearWindow();
   // построение карты теней
   m_render.attachShadowBuffer(m_shadow_frame_buffer, m_shadow_program);
   m_render.renderShape(m_ball_shadow);
   m_render.renderShape(m_plane_shadow);
   m_render.detachShadowBuffer();
   // построение сцены
   m_render.renderCubeEnvironment(m_cube);
   m_render.renderShape(m_ball);
   m_render.renderShape(m_plane);

   return(RES_S_OK);
}