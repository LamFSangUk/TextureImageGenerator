#ifndef __DATA_HEADER__
#define __DATA_HEADER__

#include <vector>
#include "pointmap.h"
using namespace std;

/* data sturcture to save Point's Information*/
typedef struct{
	double x, y, z;
}Vertex;

typedef struct{
	double x, y, z;
}Normal;

typedef struct{
	int x, y;
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

// Save the three points that are triagular's vertices
typedef struct{
	PointIdx p[3];
}TriangularMesh;

extern vector<Vertex> vertex_list;			// list for vertices
extern vector<Texture> texture_list;		// list for texture coordinates
extern vector<Normal> normal_list;			// list for point's normal vector
extern vector<TriangularMesh> trimesh_list;	// list for triangular mesh data

// Kd-tree for points
extern PointMap* points_data;

#endif