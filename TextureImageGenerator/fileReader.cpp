#include<stdio.h>
#include"data.h"

using namespace std;

vector<Vertex> v;
vector<Texture> t;
vector<Normal> n;
vector<TriangularMesh> tm;

/*	Name: readObjFile
	read obj file data *.obj
*/
void readObjFile(char* filename){
	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("file open error\n");
		return;
	}

	char line[100];
	while (!feof(fp)){
		fgets(line, 100, fp);

		if (line[0] == 'v'){
			if (line[1] == 't'){
				Texture temp;
				sscanf(line, "%*s %lf %lf", &temp.x, &temp.y);
				t.push_back(temp);
			}
			else if (line[1] == 'n'){
				Normal temp;
				sscanf(line, "%*s %lf %lf %lf", &temp.x, &temp.y, &temp.z);
				n.push_back(temp);
			}
			else{
				Vertex temp;
				sscanf(line, "%*s %lf %lf %lf", &temp.x, &temp.y, &temp.z);
				v.push_back(temp);
			}
		}
		else if (line[0] == 'f'){
			char firstpoint[100];
			char secondpoint[100];
			char thirdpoint[100];

			int vidx[3];//for vertex index
			int vtidx[3];//for texture index
			int vnidx[3];//for normal index

			TriangularMesh temp;

			sscanf(line, "%*s %s %s %s", firstpoint,secondpoint,thirdpoint);
			sscanf(firstpoint, "%d%*c%d%*c%d", &vidx[0], &vtidx[0], &vnidx[0]);
			sscanf(secondpoint, "%d%*c%d%*c%d", &vidx[1], &vtidx[1], &vnidx[1]);
			sscanf(thirdpoint, "%d%*c%d%*c%d", &vidx[2], &vtidx[2], &vnidx[2]);

			for (int i = 0; i < 3; i++){
				temp.p[i].vertexidx = vidx[i];
				temp.p[i].normalidx = vnidx[i];
				temp.p[i].textureidx = vtidx[i];
			}
			tm.push_back(temp);
		}
	}

	fclose(fp);
}

/*	Name: readPointCloud
	read pointcloud file data *.ply
*/
void readPointCloud(char* filename){
	FILE *fp = fopen(filename, "r");

	if (fp == NULL){
		printf("file open error\n");
		return;
	}

	char line[100];
	int flag = 0;

	while (fgets(line, 100, fp)){

		if (strcmp(line, "end_header\n") == 0){
			flag = 1;
			continue;
		}
		
		if (flag == 0){
			char *token=strtok(line, " ");
			if (strcmp(token,"element")==0){
				token=strtok(NULL, " ");
				if (strcmp(token, "vertex") == 0){
					token = strtok(NULL, " ");
					int num_points=atoi(token);
					points_data = new PointMap(num_points);
				}
			}
			continue;
		}

		float pos[3], normal[3];
		unsigned char color[6];
		sscanf(line, "%f %f %f %f %f %f %d %d %d", &pos[0], &pos[1], &pos[2], &normal[0], &normal[1], &normal[2], &color[0], &color[1], &color[2]);

		points_data->store((const float *)pos, (const float *)normal, (unsigned char*)color);
	}

	fclose(fp);
}

/*	Name: readBinPlyFile
	read pointcloud binary file data *.bin
*/
void readBinPlyFile(char* filename){
	FILE *fp = fopen(filename, "rb");

	if (fp == NULL){
		printf("file open error\n");
		return;
	}

	int num_points;
	float pos[3], normal[3],temp;
	float _color[6];
	unsigned char color[3];

	fread(&num_points, sizeof(int), 1, fp);

	points_data = new PointMap(num_points);

	for (int i = 0; i < num_points; i++){
		
		fread(pos, sizeof(float), 3, fp);
		fread(normal, sizeof(float), 3, fp);
		fread(_color, sizeof(float), 3, fp);
		fread(&temp, sizeof(float), 1, fp);

		for (int j = 0; j < 3; j++) color[j] = (unsigned char)(_color[j]*255);
		points_data->store((const float *)pos, (const float *)normal, (unsigned char*)color);
	}

	fclose(fp);
}