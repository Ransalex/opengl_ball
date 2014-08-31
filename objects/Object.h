//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| �����
//+------------------------------------------------------------------+
struct Point
{
   float x;
   float y;
   float z;
};
//+------------------------------------------------------------------+
//| ����������
//+------------------------------------------------------------------+
struct Coord
{
   float x;
   float y;
};
//+------------------------------------------------------------------+
//| �������
//+------------------------------------------------------------------+
struct Normal
{
   float x;
   float y;
   float z;
};
//+------------------------------------------------------------------+
//| �������������� ������
//+------------------------------------------------------------------+
struct Shape
{
   CVector<Point>  vertices;     // ��������
   CVector<Point>  indexes;      //  TODO ���� �� ������������, ����� ������� ���������� ������� �� ���������
   CVector<Coord>  text_coord;   // ���������� ��������
   CVector<Normal> normal;       // ������� ������
};
//+------------------------------------------------------------------+
//| ������ ��� ���������
//| TODO ����������
//+------------------------------------------------------------------+
struct ShapeGL
{
   size_t     vertices_size;   // ���-�� ������
   GLuint     program;         // ������ ��� �������  
   GLuint     vertex_buff;     // ����� ������
   GLuint     text_coord_buff; // ����� ��������� ���������
   GLuint     normal_buff;     // ����� ��������

   GLuint     texture;         // �������� �������
   GLuint     cube_map;        // ���������� ��������
   GLuint     texture_shadow;  // �������� ����

   GLint      uniform_texture;
   GLint      uniform_cube_map;
   GLint      uniform_texture_shadow;
   
   math::mat4 model_matrix;
   math::mat4 view_matrix;
   math::mat4 proj_matrix;
   math::mat4 shadow_matrix;
   math::vec3 light_position;

   GLint      uniform_view_matrix;
   GLint      uniform_model_matrix;
   GLint      uniform_prog_matrix;
   GLint      uniform_shadow_matrix;
   GLint      uniform_light_position;

   GLuint     vao;
};