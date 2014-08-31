//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Точка
//+------------------------------------------------------------------+
struct Point
{
   float x;
   float y;
   float z;
};
//+------------------------------------------------------------------+
//| Координаты
//+------------------------------------------------------------------+
struct Coord
{
   float x;
   float y;
};
//+------------------------------------------------------------------+
//| Нормаль
//+------------------------------------------------------------------+
struct Normal
{
   float x;
   float y;
   float z;
};
//+------------------------------------------------------------------+
//| Геометрический объект
//+------------------------------------------------------------------+
struct Shape
{
   CVector<Point>  vertices;     // веришины
   CVector<Point>  indexes;      //  TODO пока не используется, нужно сделать построение индесов по веришинам
   CVector<Coord>  text_coord;   // координаты текстуры
   CVector<Normal> normal;       // номарли вершин
};
//+------------------------------------------------------------------+
//| Объект для отрисовки
//| TODO переделать
//+------------------------------------------------------------------+
struct ShapeGL
{
   size_t     vertices_size;   // кол-во вершин
   GLuint     program;         // шейдер для объекта  
   GLuint     vertex_buff;     // буфер данных
   GLuint     text_coord_buff; // буфер текстурнз координат
   GLuint     normal_buff;     // буфер нормалей

   GLuint     texture;         // текстура объекта
   GLuint     cube_map;        // кубическая текстура
   GLuint     texture_shadow;  // текстура тени

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