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

typedef struct PointCoord{
	int x, y;
	bool operator==(struct PointCoord a){
		if (a.x == x && a.y == y)
			return true;
		else
			return false;
	}
}PointCoord;

typedef struct{
	int vertexidx;
	int normalidx;
	int textureidx;
}PointIdx;

typedef struct{
	PointIdx p[3];
}TriangularMesh;

extern vector<Vertex> v;
extern vector<Texture> t;
extern vector<Normal> n;

extern vector<TriangularMesh> tm;

#endif