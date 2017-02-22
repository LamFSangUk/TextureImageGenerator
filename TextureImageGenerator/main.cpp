#include<stdio.h>
#include"data.h"

void readFromFile(char* filename);

int main(void){

	char *filename = "carModel_uv.obj";
	readFromFile(filename);

	return 0;
}