#include<stdio.h>
#include<stdlib.h>
#include"data.h"
#include"operation.h"
#include"pointmap.h"
#include"bmp.h"

void readObjFile(char* filename);
void readPointCloud(char* filename);
void readBinFile(char* filename);

PointMap* points_data;

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
	//readPointCloud(pointcloudfilename);
	readBinFile(pointcloudfilename);
	printf("**************** POINTCLOUD READ COMPLETE ****************\n");

	points_data->balance();

	unsigned char *img;
	bool *imgflag;
	int width, height;
	width = height = 2048;
	//2048x1152//1920*1080//1280x720//4096*2160
	img = (unsigned char *)calloc(3 * width*height,1);
	imgflag = (bool*)calloc(width*height, sizeof(bool));

	paintPicture(img, imgflag, width, height);

	imgKernel(img, imgflag, width, height);

	//Create BMP image file, and write img arr to it.
	makeBmpImage(width, height, img);
	printf("******************* GENERATE BMP IMAGE *******************\n");

	return 0;
}