//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "GLWindow.h"

CScene CGLWindow::m_scene;
int CGLWindow::m_button_state=0;
int CGLWindow::m_mouse_x=0;
int CGLWindow::m_mouse_y=0;
//+------------------------------------------------------------------+
//| Конструктор
//+------------------------------------------------------------------+
CGLWindow::CGLWindow()
{
   ZeroMemory(&m_window_info, sizeof(m_window_info));
}
//+------------------------------------------------------------------+
//| Декструктор
//+------------------------------------------------------------------+
CGLWindow::~CGLWindow()
{

}
//+------------------------------------------------------------------+
//| Создание окна
//+------------------------------------------------------------------+
 SCFRESULT CGLWindow::createOpenGLWindow(const char* window_title, const int width, const int height, const bool fullScreen)
{
   if(width<0 || height<0) ReturnError(RES_E_INVALID_ARGS);

   if (!glfwInit())
      ReturnError(RES_E_OPENGL_ERRROR);
    
   m_window_info.width     =width;
   m_window_info.height    =height;
   m_window_info.fullscreen=fullScreen;
    
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
   glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);    
   glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 6);
   glfwOpenWindowHint(GLFW_STEREO, GL_FALSE);

   if(m_window_info.fullscreen)
   {
      if(m_window_info.width == 0 || m_window_info.height == 0)
      {
         GLFWvidmode mode;
         glfwGetDesktopMode(&mode);
         m_window_info.width = mode.Width;
         m_window_info.height = mode.Height;
      }
      glfwOpenWindow(m_window_info.width, m_window_info.height, 8, 8, 8, 0, 32, 0, GLFW_FULLSCREEN);
   }
   else
   {
      if(!glfwOpenWindow(m_window_info.width, m_window_info.height, 8, 8, 8, 0, 32, 0, GLFW_WINDOW))
      {
         ReturnError(RES_E_OPENGL_ERRROR);
      }
   }

   glfwSetWindowTitle(window_title);
   glfwSetMouseButtonCallback(onMouseButton);
   glfwSetWindowSizeCallback(onResize);
   glfwSetMousePosCallback(onMouseMove);
   gl3wInit();

   return(RES_S_OK);
 }
//+------------------------------------------------------------------+
//| Каллбэк на изменение размера окна
//+------------------------------------------------------------------+
void GLFWCALL CGLWindow::onResize(int width, int hegiht)
{
   m_scene.resizeWindow(width, hegiht);
}
//+------------------------------------------------------------------+
//| Каллбэк на движение мыши
//+------------------------------------------------------------------+
void GLFWCALL CGLWindow::onMouseMove(int x, int y)
{   
   if(m_button_state&MOUSE_BUTTON_LEFT)
   {
      m_scene.moveCamera(x-m_mouse_x);
      m_scene.moveObject(x-m_mouse_x, y-m_mouse_y);      
   }

   m_mouse_x=x;
   m_mouse_y=y;
}
//+------------------------------------------------------------------+
//| Каллбэк на клик
//+------------------------------------------------------------------+
void GLFWCALL CGLWindow::onMouseButton(int button, int action)
{
   switch(button)
   {
      case 0:
         if(action)
         {
            m_scene.selectObject(m_mouse_x, m_mouse_y);

            m_button_state |=MOUSE_BUTTON_LEFT;
         }
         else
         {
            m_scene.unselectObject();

            m_button_state &=~MOUSE_BUTTON_LEFT;
         }
         break;
      case 1:
         if(action)
            m_button_state |=MOUSE_BUTTON_REIGHT;
         else
            m_button_state &=~MOUSE_BUTTON_REIGHT;
         break;
      case 2:
         if(action)
            m_button_state |=MOUSE_BUTTON_CENTER;
         else
            m_button_state &=~MOUSE_BUTTON_CENTER;
         break;
      default:
         break;
   }
}
//+------------------------------------------------------------------+
//| Главный цикл
//+------------------------------------------------------------------+
SCFRESULT CGLWindow::startMainLoop()
{
   double deltaTime     =0;
   double beginFrameTime=0;
   double fixedTimeStep =1.0 / 100.0;

   m_window_info.running = true; 
   m_window_info.active  = true;
  
   m_scene.initialize(m_window_info.width, m_window_info.height); 

   do
   {
      m_scene.update(glfwGetTime());      
      m_scene.render();

      glfwSwapBuffers();

      m_window_info.running &= (glfwGetKey( GLFW_KEY_ESC ) == GLFW_RELEASE);
      m_window_info.running &= (glfwGetWindowParam( GLFW_OPENED ) != GL_FALSE);

   } while(m_window_info.running);

   glfwTerminate();

   return(RES_S_OK);
}
