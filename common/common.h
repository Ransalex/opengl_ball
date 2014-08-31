//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once

enum SCFRESULT
{
   RES_E_FAIL          = -1, // общая ошибка
   RES_E_INVALID_ARGS  = -3, // переданные параметры не корректны
   RES_E_OUT_OF_MEMORY = -4, // для выполнения операции не хватает памяти
   RES_E_OPENGL_ERRROR = -5, // ошибка OpenGL
   RES_S_OK            =  0
};

#define RES_FAIL(res)    ((res)<0)
#define RES_SUCCESS(res) ((res)>=0)

#define LOG_ERROR(...) 

#define ReturnError(result) return(ReturnErrorT(__FUNCTION__, __LINE__,  NULL, result), assert(0), result)

template <typename T> void ReturnErrorT(const char *function, int line, const char *comment, const T &result)
{
   //ExtLogger(context, LOG_STATUS_DEBUG) << (function ? function : "UNKNOWN") << ":" << line << " " << (comment ? comment : "failed") << " [" << result << "]";
}

inline void DebugOutput(const char* szFormat, ...)
{
   char szBuff[1024];
   va_list arg;
   va_start(arg, szFormat);
   _vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
   va_end(arg);

   OutputDebugString(szBuff);
}