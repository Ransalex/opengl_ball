//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "render/GLWindow.h"

int main(int argc, char argv[])
{
   SCFRESULT result=RES_S_OK;
   CGLWindow window;

   window.createOpenGLWindow("Scyforge test", 800, 600, false);

   result=window.startMainLoop();
      
	return(0);
}