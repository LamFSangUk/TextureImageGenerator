#ifndef __OPERATION_HEADER__
#define __OPERATION_HEADER__

#include<stdio.h>
#include<cmath>
#include"data.h"

#define Vector Vertex

double DotProduct(Normal, const double *);
void paintTriangle(unsigned char *,bool *, int*, int, int, PointCoord a, PointCoord b, PointCoord c);
void paintImage(unsigned char *,bool *, int, int);
void imgPostProcessing(unsigned char *, bool*, int, int);

#endif