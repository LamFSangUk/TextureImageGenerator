#include<stdio.h>
#include<stdlib.h>
#include"data.h"
#include"operation.h"
#include"pointmap.h"
#include"bmp.h"

void readFromFile(char* filename);
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

	readFromFile(objfilename);
	printf("***************** OBJ FILE READ COMPLETE *****************\n");
	readPointCloud(pointcloudfilename);
	printf("**************** POINTCLOUD READ COMPLETE ****************\n");

	points_data.balance();

	unsigned char *img;
	int width=1920, height=1080;

	img = (unsigned char *)calloc(3 * width*height,1);

	paintPicture(img, width, height);

	makeBmpImage(width, height, img);

	return 0;
}