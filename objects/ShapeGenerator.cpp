//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#include "stdafx.h"

#include "ShapeGenerator.h"
//+------------------------------------------------------------------+
//| Конструктор
//+------------------------------------------------------------------+
CShapeGenerator::CShapeGenerator()
{

}
//+------------------------------------------------------------------+
//| Деструктор
//+------------------------------------------------------------------+
CShapeGenerator::~CShapeGenerator()
{

}
//+------------------------------------------------------------------+
//| Генерация сферы
//+------------------------------------------------------------------+
SCFRESULT CShapeGenerator::createSphere(const float radius, const int rings, const int sectors, Shape &shape)
{
   if(radius<=0 || rings<=0 || sectors<=0) ReturnError(RES_E_INVALID_ARGS);

   float const rings_step   = 1.0f/(float)(rings-1);
   float const sectors_step = 1.0f/(float)(sectors-1);
   size_t      point_cout=0;
   CVector<Point>  pointers;
   CVector<Normal> pointers_normals;
   CVector<Coord>  text_coords;

   shape.vertices.clear();
   shape.normal.clear();
   shape.text_coord.clear();
   shape.indexes.clear();
   
   pointers.reserve(rings *sectors);
   pointers_normals.reserve(rings *sectors);
   text_coords.reserve(rings *sectors);   

   // строим точки
   for(int r = 0; r < rings; ++r)
   {
      float const y = (float)sin( -M_PI_2 + M_PI * r * rings_step );

      for(int s = 0; s < sectors; ++s)
      {
         float const x = (float)cos(2.0f*M_PI * s * sectors_step) * (float)sin( M_PI * r * rings_step );
         float const z = (float)sin(2.0f*M_PI * s * sectors_step) * (float)sin( M_PI * r * rings_step );

         Coord coord={s*sectors_step, r*rings_step};
         text_coords.push(coord);

         Point point={ x * radius, y * radius, z * radius};
         pointers.push(point);

         Normal normal={x,y,z};
         pointers_normals.push(normal);
      }
   }

   point_cout=pointers.size();

   shape.vertices.reserve(rings * sectors * 3);
   shape.normal.reserve(rings * sectors * 3);
   shape.text_coord.reserve(rings * sectors * 3);

   point_cout=0;
   
   // заполяем вершины треугольников
   // TODO передеintлать на заполнение индексов вершин
   for(size_t i=0, size=pointers.size(); i<size; ++i, ++point_cout)
   {
      if(point_cout>=(size_t)sectors)
      {
         if((point_cout%sectors)!=0)
         {
            shape.vertices.push( pointers[point_cout-sectors] );                 
            shape.vertices.push( pointers[point_cout-1] );   
            shape.vertices.push( pointers[point_cout] );

            shape.text_coord.push( text_coords[point_cout-sectors] );
            shape.text_coord.push( text_coords[point_cout-1] );
            shape.text_coord.push( text_coords[point_cout] );

            shape.normal.push( pointers_normals[point_cout-sectors] );
            shape.normal.push( pointers_normals[point_cout-1] );
            shape.normal.push( pointers_normals[point_cout] );
         }

         shape.vertices.push( pointers[point_cout-sectors+1] );
         shape.vertices.push( pointers[point_cout-sectors] );   
         shape.vertices.push( pointers[point_cout] );

         shape.text_coord.push( text_coords[point_cout-sectors+1] );
         shape.text_coord.push( text_coords[point_cout-sectors] );
         shape.text_coord.push( text_coords[point_cout] );

         shape.normal.push( pointers_normals[point_cout-sectors+1] );
         shape.normal.push( pointers_normals[point_cout-sectors] );
         shape.normal.push( pointers_normals[point_cout] );
      }
   }

   return(RES_S_OK);
}
//+------------------------------------------------------------------+
//| Генерация сферы
//+------------------------------------------------------------------+
SCFRESULT CShapeGenerator::createSquarePlane(const float size, Shape &shape)
{
   if(size<=0) ReturnError(RES_E_INVALID_ARGS);

   Point point={0};

   float side=size/2;

   point.x=-side; point.y=0;point.z=-side;
   shape.vertices.push(point);
   point.x=side; point.y=0;point.z=side;
   shape.vertices.push(point);  
   point.x=-side; point.y=0;point.z=side;
   shape.vertices.push(point);
   point.x=-side; point.y=0;point.z=-side;
   shape.vertices.push(point);   
   point.x=side; point.y=0;point.z=-side;
   shape.vertices.push(point);    
   point.x=side; point.y=0;point.z=side;
   shape.vertices.push(point);

   Coord coord={0};

   coord.x=0; coord.y=0;
   shape.text_coord.push(coord);
   coord.x=5; point.z=5;
   shape.text_coord.push(coord);   
   coord.x=0; coord.y=5;
   shape.text_coord.push(coord);
   coord.x=0; coord.y=0;
   shape.text_coord.push(coord);
   coord.x=5; coord.y=5;
   shape.text_coord.push(coord);   
   coord.x=5; coord.y=0;   
   shape.text_coord.push(coord);
   
   return(RES_S_OK);
}