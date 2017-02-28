#include<stdio.h>
//#include<set>
//#include<string>
#include"data.h"
#include"pointmap.h"

using namespace std;

vector<Vertex> v;
vector<Texture> t;
vector<Normal> n;
vector<TriangularMesh> tm;

//set<string> _pointcloud;

extern PointMap points_data;

void readFromFile(char* filename){
	FILE *fp = fopen(filename, "r");
	//FILE *test = fopen("test.txt", "w");

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
				//fprintf(test,"vt %f %f\n", temp.x, temp.y);
				t.push_back(temp);
			}
			else if (line[1] == 'n'){
				Normal temp;
				sscanf(line, "%*s %lf %lf %lf", &temp.x, &temp.y, &temp.z);
				//fprintf(test,"vn %f %f %f\n", temp.x, temp.y, temp.z);
				n.push_back(temp);
			}
			else{
				Vertex temp;
				sscanf(line, "%*s %lf %lf %lf", &temp.x, &temp.y, &temp.z);
				//fprintf(test,"v %f %f %f\n", temp.x, temp.y, temp.z);
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

			//Make the PointCloud
			//_pointcloud.insert(firstpoint);
			//_pointcloud.insert(secondpoint);
			//_pointcloud.insert(thirdpoint);

			for (int i = 0; i < 3; i++){
				temp.p[i].vertexidx = vidx[i];
				//printf("%d /", vtidx[i]);
				temp.p[i].normalidx = vnidx[i];
				temp.p[i].textureidx = vtidx[i];
			}
			tm.push_back(temp);
		}
	}

	//printf("size: %d\n", _pointcloud.size());

	fclose(fp);
}

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

		if (flag == 0)
			continue;

		float pos[3], normal[3];
		unsigned char color[6];
		sscanf(line, "%f %f %f %f %f %f %d %d %d", &pos[0], &pos[1], &pos[2], &normal[0], &normal[1], &normal[2], &color[0], &color[1], &color[2]);
		//sscanf(line, "%f %f %f %f %f %f %d %d %d", &pos[0], &pos[1], &pos[2], &normal[0], &normal[1], &normal[2], &color[0], &color[1], &color[2]);

		//printf("%f %f %d\n", pos[0], normal[0], color[0]);
		points_data.store((const float *)pos, (const float *)normal, (unsigned char*)color);
	}

	fclose(fp);
}