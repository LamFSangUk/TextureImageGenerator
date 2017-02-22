#include<stdio.h>
#include"data.h"

vector<Vertex> v;
vector<Texture> t;
vector<Normal> n;
vector<TriangularMesh> tm;

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
			char vertexset[100];
			char textureset[100];
			char normalset[100];

			int vidx[3];//for vertex index
			int vtidx[3];//for texture index
			int vnidx[3];//for normal index

			TriangularMesh temp;

			sscanf(line, "%*s %s %s %s", vertexset, textureset, normalset);
			sscanf(vertexset, "%d%*c%d%*c%d", &vidx[0], &vidx[1], &vidx[2]);
			sscanf(textureset, "%d%*c%d%*c%d", &vtidx[0], &vtidx[1], &vtidx[2]);
			sscanf(normalset, "%d%*c%d%*c%d", &vnidx[0], &vnidx[1], &vnidx[2]);

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