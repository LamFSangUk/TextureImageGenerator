#ifndef __OPERATION_HEADER__
#define __OPERATION_HEADER__

#include<stdio.h>
#include<cmath>
#include"data.h"
#include"pointmap.h"

#define Vector Vertex
#define PointCoord Texture

double SignedArea(const Texture& p, const Texture& q, const Texture& r);
Vector CrossProduct(Vector,Vector);
Vector TextureSubstraction(const Texture&, const Texture&);
Texture getBarycentric(double *, double *, double *, const Texture&, const Texture&, const Texture&);
double getDist(const float *, const float *);
void paintTriangle(unsigned char *,PointCoord a, PointCoord b, PointCoord c);
void paintPicture(unsigned char *,int, int);

#endif