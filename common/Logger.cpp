//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "Logger.h"
//+------------------------------------------------------------------+
//| Конструктор
//+------------------------------------------------------------------+
CLogger::CLogger():m_file(NULL)
{
   
}
//+------------------------------------------------------------------+
//| Деструктор
//+------------------------------------------------------------------+
CLogger::~CLogger()
{
   if(m_file!=NULL)
      fclose(m_file);

   m_file=NULL;
}
//+------------------------------------------------------------------+
//| Создание лог файла
//+------------------------------------------------------------------+
SCFRESULT CLogger::Create(const char *fileName)
{
//    if(fileName==NULL) return(RES_E_INVALID_ARGS);
// 
//    if ((m_file = fopen(fileName, "w"))== NULL)
//      {
//       return(RES_E_FAIL);
//      }

   return RES_S_OK;
}
//+------------------------------------------------------------------+
//| Запись в лог 
//+------------------------------------------------------------------+
SCFRESULT CLogger::LoggerWrite(const char *format, ...)
{
//    if(format==NULL || m_file==NULL) return(RES_E_INVALID_ARGS);
// 
// 	va_list ap;
//    va_start(ap, format);
// 
// 	if(RES_FAIL(vfprintf(m_file, format, ap)))
//       return(RES_E_FAIL);
// 	
//    va_end(ap);

   return(RES_S_OK);
}
