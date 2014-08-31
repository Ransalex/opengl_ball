//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "TextureLoader.h"

//+------------------------------------------------------------------+
//| Структура заголовка KTX файла
//+------------------------------------------------------------------+
struct KTXheader
{
   unsigned char identifier[12];
   unsigned int  endianness;
   unsigned int  gltype;
   unsigned int  gltypesize;
   unsigned int  glformat;
   unsigned int  glinternalformat;
   unsigned int  glbaseinternalformat;
   unsigned int  pixelwidth;
   unsigned int  pixelheight;
   unsigned int  pixeldepth;
   unsigned int  arrayelements;
   unsigned int  faces;
   unsigned int  miplevels;
   unsigned int  keypairbytes;
};
//+------------------------------------------------------------------+
//| Конструктор
//+------------------------------------------------------------------+
CTextureLoader::CTextureLoader()
{
}
//+------------------------------------------------------------------+
//| Деструктор
//+------------------------------------------------------------------+
CTextureLoader::~CTextureLoader()
{
}
//+------------------------------------------------------------------+
//| Загрузка текстуры из PNG файла
//+------------------------------------------------------------------+
SCFRESULT CTextureLoader::loadTexturePng(const char* filename, GLuint *texture)
  {
   if(filename==NULL || texture==NULL) ReturnError(RES_E_INVALID_ARGS);

   int            widtht =0;
   int            heightt=0;;
   GLenum         err    =0;
   GLuint         retval =0;
   unsigned char* image  =0;

   *texture=0;

   glGenTextures(1, &retval);
   glBindTexture(GL_TEXTURE_2D, retval);
     
   image = SOIL_load_image(filename, &widtht, &heightt, 0, SOIL_LOAD_RGB);
   if(image==NULL)
      ReturnError(RES_E_FAIL);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widtht, heightt, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

   SOIL_free_image_data(image);

   glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   *texture=retval;

   err=glGetError();
   return( err ? RES_E_OPENGL_ERRROR : RES_S_OK );
  }
//+------------------------------------------------------------------+
//| Загрузка кубической KTX текстуры из файла
//| http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec
//+------------------------------------------------------------------+
SCFRESULT CTextureLoader::loadTextureCubic(const char* filename, GLuint *texture)
  {
   if(filename==NULL || texture==NULL) ReturnError(RES_E_INVALID_ARGS);

   int    widtht   =0;
   int    heightt  =0;
   GLenum err      =0;
   GLuint temp     =GL_NONE;
   GLuint retval   =GL_NONE;
   FILE  *fp       =NULL;
   KTXheader header={0};
   size_t data_start=0;
   size_t data_end  =0;
   GLenum target   = GL_NONE;

   *texture=0;   

   fp = fopen(filename, "rb");
   if(!fp)
      ReturnError(RES_E_FAIL);

   if(fread(&header, sizeof(header), 1, fp) != 1)
      ReturnError(RES_E_FAIL);

   if(header.miplevels==0)
      header.miplevels = 1;

   data_start = ftell(fp) + header.keypairbytes;
   fseek(fp, 0, SEEK_END);
   data_end = ftell(fp);
   fseek(fp, data_start, SEEK_SET);

    CSmartArray<unsigned char> data=new(std::nothrow) unsigned char[data_end - data_start];
    if(data==NULL)
      {
       fclose(fp);
       ReturnError(RES_E_OUT_OF_MEMORY);
      }
    
    ZeroMemory(data, data_end - data_start);
    
    fread(data, 1, data_end - data_start, fp);

    glGenTextures(1, &retval);
    glBindTexture(GL_TEXTURE_CUBE_MAP, retval);

    glTexStorage2D(GL_TEXTURE_CUBE_MAP, header.miplevels, header.glinternalformat, header.pixelwidth, header.pixelheight);

    unsigned int face_size = calculate_face_size(header);
    for(unsigned int i=0; i<header.faces; ++i)
    {
       glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, header.pixelwidth, header.pixelheight, header.glformat, header.gltype, data + face_size * i);
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

   fclose(fp);

   *texture=retval;

   err=glGetError();
   return( err ? RES_E_OPENGL_ERRROR : RES_S_OK );
  }
//+------------------------------------------------------------------+
//| Генерация текстуры для теней
//+------------------------------------------------------------------+
SCFRESULT CTextureLoader::generateShadowTexture(GLuint *shadow_frame_buffer, GLuint *texture)
{
   if(texture==NULL || shadow_frame_buffer==NULL) ReturnError(RES_E_INVALID_ARGS);

   GLuint retval=0;

   glGenFramebuffers(1, shadow_frame_buffer);
   glBindFramebuffer(GL_FRAMEBUFFER, *shadow_frame_buffer);

   glGenTextures(1, &retval);
   glBindTexture(GL_TEXTURE_2D, retval);

   glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, retval, 0);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   *texture=retval;

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Расчет шага стороны текстуры
//+------------------------------------------------------------------+
unsigned int CTextureLoader::calculate_stride(const KTXheader& h, unsigned int width, unsigned int pad)
{
   unsigned int channels=0;

   switch (h.glbaseinternalformat)
   {
      case GL_RED:    channels = 1;
          break;
      case GL_RG:     channels = 2;
          break;
      case GL_BGR:
      case GL_RGB:    channels = 3;
          break;
      case GL_BGRA:
      case GL_RGBA:   channels = 4;
          break;
   }

   unsigned int stride = h.gltypesize * channels * width;

   stride = (stride + (pad - 1)) & ~(pad - 1);

   return(stride);
}
//+------------------------------------------------------------------+
//| Расчет размера стороны текстуры
//+------------------------------------------------------------------+
unsigned int CTextureLoader::calculate_face_size(const KTXheader& h)
{
    unsigned int stride = calculate_stride(h, h.pixelwidth);

    return(stride*h.pixelheight);
}