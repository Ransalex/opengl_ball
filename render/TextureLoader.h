//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Класс для загрузки текстур
//+------------------------------------------------------------------+
class CTextureLoader
{
public:
   CTextureLoader();
   ~CTextureLoader();

   SCFRESULT loadTexturePng(const char* filename, GLuint *texture);
   SCFRESULT loadTextureCubic(const char* filenae, GLuint *texture);
   SCFRESULT generateShadowTexture(GLuint *shadow_frame_buffer, GLuint *texture);

private:
   static unsigned int calculate_stride(const struct KTXheader& h, unsigned int width, unsigned int pad = 4);
   static unsigned int calculate_face_size(const KTXheader& h);
};
