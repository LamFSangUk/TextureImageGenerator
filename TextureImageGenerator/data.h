#ifndef __DATA_HEADER__
#define __DATA_HEADER__

#include<vector>
using namespace std;

typedef struct{
	double x, y, z;
}Vertex;

typedef struct{
	double x, y, z;
}Normal;

typedef struct{
	double x, y;
}Texture;

typedef struct{
	int vertexidx;
	int normalidx;
	int textureidx;
}Point;

typedef struct{
	Point p[3];
}TriangularMesh;

extern vector<Vertex> v;
extern vector<Texture> t;
extern vector<Normal> n;

extern vector<TriangularMesh> tm;

#endif