#include<stdio.h>
#include<stdlib.h>
#include"data.h"
#include"operation.h"
#include"pointmap.h"
#include"bmp.h"

void readObjFile(char* filename);
void readPointCloud(char* filename);

PointMap points_data(3050641);

int main(void){

	char objfilename[256]; //= "carModel_uv.obj";
	char pointcloudfilename[256]; //= "ÀÚµ¿Â÷¸ðµ¨_PointCloud_color.ply";
	
	printf("******************* TEXTURE GENERATOR ********************\n");
	printf("input the obj file name : ");
	scanf("%s", objfilename);
	printf("input the pointcloud file name : ");
	scanf("%s", pointcloudfilename);

	readObjFile(objfilename);
	printf("***************** OBJ FILE READ COMPLETE *****************\n");
	readPointCloud(pointcloudfilename);
	printf("**************** POINTCLOUD READ COMPLETE ****************\n");

	points_data.balance();

	unsigned char *img;
	int width=4096, height=2160;
	//2048x1152
	img = (unsigned char *)calloc(3 * width*height,1);

	paintPicture(img, width, height);

	//Create BMP image file, and write img arr to it.
	makeBmpImage(width, height, img);
	printf("******************* GENERATE BMP IMAGE *******************\n");

	return 0;
}