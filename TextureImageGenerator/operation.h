#ifndef __OPERATION_HEADER__
#define __OPERATION_HEADER__

#include<stdio.h>
#include<cmath>
#include"data.h"

#define Vector Vertex

double SignedArea(const Texture& p, const Texture& q, const Texture& r);
Vector CrossProduct(Vector,Vector);
Vector TextureSubstraction(const Texture&, const Texture&);
Texture getBarycentric(double *, double *, double *, const Texture&, const Texture&, const Texture&);

#endif