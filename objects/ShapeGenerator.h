//+------------------------------------------------------------------+
//|                                                   Scyforge test |
//|                                                            2014 |
//+------------------------------------------------------------------+
#pragma once

#include "Object.h"
//+------------------------------------------------------------------+
//| Класс для генерации фигур
//+------------------------------------------------------------------+
class CShapeGenerator
{
public:
    CShapeGenerator();
    ~CShapeGenerator();

    static SCFRESULT createSphere(const float radius, const int rings, const int sectors, Shape &shape);
    static SCFRESULT createSquarePlane(const float size, Shape &shape);
};
