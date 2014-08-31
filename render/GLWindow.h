//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once

#include "Scene.h"
//+------------------------------------------------------------------+
//| Состояние окна
//+------------------------------------------------------------------+
struct GLWindow
{
   int  width;
   int  height;
   bool fullscreen;
   bool active;
   bool running;
};
//+------------------------------------------------------------------+
//| Класс управления окном
//+------------------------------------------------------------------+
class CGLWindow
{
private:
   enum
   {
      MOUSE_BUTTON_LEFT   =0x0001,
      MOUSE_BUTTON_REIGHT =0x0002,
      MOUSE_BUTTON_CENTER =0x0004
   };

   GLWindow m_window_info;
  
   static CScene  m_scene;

   static int m_button_state;
   static int m_mouse_x;
   static int m_mouse_y;

public:
   CGLWindow();
   ~CGLWindow();

   SCFRESULT            createOpenGLWindow(const char* window_title, const int width, const int height, const bool fullScreen);
   SCFRESULT            startMainLoop();  

private:
   static void GLFWCALL onResize(int width, int height);
   static void GLFWCALL onMouseMove(int x, int y);
   static void GLFWCALL onMouseButton(int button, int action);
};