#include<stdio.h>
#include<stdlib.h>
#include"data.h"
#include"operation.h"
#include"pointmap.h"
#include"bmp.h"
#include<time.h>

#define CHECK_TIME true

void readObjFile(char* filename);
void readPointCloud(char* filename);
void readBinPlyFile(char* filename);

PointMap* points_data;

int main(void){

	char filename[256]; 
	char fext_obj[5] = ".obj";
	char fext_ply[5] = ".ply";
	char fext_bin[5] = ".bin";
	
	char _objfilename[256];
	char _plyfilename[256];
	clock_t tStart, tPrev;


	printf("******************* TEXTURE GENERATOR ********************\n");
	if (CHECK_TIME){
		tStart = tPrev = clock();
		printf("\t- START measuring execution time\n");
	}

	//Input the filename
	printf("input the obj file name : ");
	scanf("%s", filename);

	strcpy(_objfilename, filename);
	strcpy(_plyfilename, filename);
	strcat(_objfilename, fext_obj);
	//strcat(_plyfilename, fext_ply);
	strcat(_plyfilename, fext_bin);

	readObjFile(_objfilename);
	printf("***************** OBJ FILE READ COMPLETE *****************\n");
	if (CHECK_TIME){
		printf("\t- Read the OBJ File in %.2fs\n", (double)(clock() - tPrev) / CLOCKS_PER_SEC);
		tPrev = clock();
	}

	//if using the ply file
	//readPointCloud(pointcloudfilename);

	//if using the bin ply file
	readBinPlyFile(_plyfilename);
	printf("************* POINTCLOUD FILE READ COMPLETE **************\n");
	if (CHECK_TIME){
		printf("\t- Read the PLY File in %.2fs\n", (double)(clock() - tPrev) / CLOCKS_PER_SEC);
		tPrev = clock();
	}

	//balance the kd-tree
	points_data->balance();
	printf("****************** BALANCED THE KD-TREE ******************\n");
	if (CHECK_TIME){
		printf("\t- Balance the kd-tree in %.2fs\n\n", (double)(clock() - tPrev) / CLOCKS_PER_SEC);
		tPrev = clock();
	}

	unsigned char *img_res;
	bool *img_flag;
	int width, height;
	width = height = 1024;
	//2048x1152//1920*1080//1280x720//4096*2160
	img_res = (unsigned char *)calloc(3 * width*height,1);
	img_flag = (bool*)calloc(width*height, sizeof(bool));

	paintPicture(img_res, img_flag, width, height);
	if (CHECK_TIME){
		printf("\t- Painting the Texture img in %.2fs\n", (double)(clock() - tPrev) / CLOCKS_PER_SEC);
		tPrev = clock();
	}

	imgKernel(img_res, img_flag, width, height);
	if (CHECK_TIME){
		printf("\t- After processing with texture img in %.2fs\n", (double)(clock() - tPrev) / CLOCKS_PER_SEC);
		tPrev = clock();
	}


	//Create BMP image file, and write img_res arr to it.
	makeBmpImage(width, height, img_res);
	printf("******************* GENERATE BMP IMAGE *******************\n");
	if (CHECK_TIME){
		printf("\t- Generate the target bmp file in %.2fs\n", (double)(clock() - tPrev) / CLOCKS_PER_SEC);
		tPrev = clock();

		printf("\n\t-- Total Elapsed Time = %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	}

	return 0;
}