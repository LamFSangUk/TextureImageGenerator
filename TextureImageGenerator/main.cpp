#include<stdio.h>
#include"data.h"
#include"operation.h"
#include"pointmap.h"

void readFromFile(char* filename);
void readPointCloud(char* filename);

//FILE*test;

//extern vector<Vertex> v;
//extern vector<Texture> t;
//extern vector<Normal> n;
//
//extern vector<TriangularMesh> tm;

PointMap points_data(3050641);

int main(void){

	char *objfilename = "carModel_uv.obj";
	char *pointcloudfile = "ÀÚµ¿Â÷¸ðµ¨_PointCloud_color.ply";
	
	//points_data=PointMap;

	readFromFile(objfilename);
	printf("**************** OBJ FILE READ COMPLETE ****************\n");
	readPointCloud(pointcloudfile);
	printf("**************** POINTCLOUD READ COMPLETE ****************\n");

	points_data.balance();

	int i, len;

	float max_dist = 10000.0;
	int npoints = 10;

	//test = fopen("test.dat", "w");

	len = tm.size();
	//printf("%d\n", len);
	for (i = 0; i < 10; i++){
		Texture tc1, tc2, tc3, qtc;
		Vertex qm,v1,v2,v3;
		Normal nm,n1,n2,n3;
		double alpha, beta, gamma;
		tc1 = t[tm[i].p[0].textureidx - 1];
		tc2 = t[tm[i].p[1].textureidx - 1];
		tc3 = t[tm[i].p[2].textureidx - 1];

		//printf("%lf %lf \n", tc1.x, tc1.y);

		v1 = v[tm[i].p[0].vertexidx - 1];
		v2 = v[tm[i].p[1].vertexidx - 1];
		v3 = v[tm[i].p[2].vertexidx - 1];

		n1 = n[tm[i].p[0].normalidx - 1];
		n2 = n[tm[i].p[1].normalidx - 1];
		n3 = n[tm[i].p[2].normalidx - 1];

		qtc = getBarycentric(&alpha, &beta, &gamma, tc1, tc2, tc3);
		
		//printf("%lf %lf %lf//\n", alpha, beta, gamma);

		qm.x = alpha*v1.x + beta*v2.x + gamma*v3.x;
		qm.y = alpha*v1.y + beta*v2.y + gamma*v3.y;
		qm.z = alpha*v1.z + beta*v2.z + gamma*v3.z;

		nm.x = alpha*n1.x + beta*n2.x + gamma*n3.x;
		nm.y = alpha*n1.y + beta*n2.y + gamma*n3.y;
		nm.z = alpha*n1.z + beta*n2.z + gamma*n3.z;

		NearestPoints np;

		np.dist2 = (float*)alloca(sizeof(float)*(npoints + 1));
		np.index = (const Point**)alloca(sizeof(Point*)*(npoints + 1));

		np.pos[0] = qm.x; np.pos[1] = qm.y; np.pos[2] = qm.z;
		np.max = npoints;
		np.found = 0;
		np.got_heap = 0;
		np.dist2[0] = max_dist*max_dist;

		points_data.locate_points(&np, 1);

		//check
		printf("check : %d %lf %lf %lf\n", i, qm.x, qm.y, qm.z);
		printf("%d/", np.found);
		for (int i = 1; i <= np.found; i++)
			printf("%f %f %f %u %u %u\n", np.index[i]->pos[0], np.index[i]->pos[1], np.index[i]->pos[2], np.index[i]->color[0], np.index[i]->color[1], np.index[i]->color[2]);
		printf("\n");


		

	}

	return 0;
}