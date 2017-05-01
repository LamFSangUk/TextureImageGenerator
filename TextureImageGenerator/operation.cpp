#include "operation.h"

//PointMap* points_data;

double SignedArea(const Texture& p, const Texture& q, const Texture& r){
	double result;
	Vector temp1, temp2, temp3;
	temp1=TextureSubstraction(q, p); //Find the Vector 1
	temp2=TextureSubstraction(r, p); //Find the Vector 2

	// Calculate the triangle's area by cross product between Vector 1 and Vector 2
	temp3=CrossProduct(temp1, temp2); 
	result = sqrt(temp3.z*temp3.z)/2; 

	if (result<0) result = (-result);

	return result;
}
Vector CrossProduct(Vector a,Vector b){
	Vector vec;
	vec.x = a.y*b.z-a.z*b.y;
	vec.y = -(a.x*b.z-a.z*b.x);
	vec.z = a.x*b.y - a.y*b.x;

	return vec;
}
Vector TextureSubstraction(const Texture& a, const Texture& b){
	Vector vec;
	vec.x = a.x - b.x;
	vec.y = a.y - b.y;
	vec.z = 0;

	return vec;
}

Texture getBarycentric(double *alpha, double *beta, double*gamma, const Texture& tc1, const Texture& tc2, const Texture& tc3){
	double originSA = SignedArea(tc1, tc2, tc3);
	Texture qtc;

	qtc.x = (tc1.x + tc2.x + tc3.x) / 3;
	qtc.y = (tc1.y + tc2.y + tc3.y) / 3;

	*alpha = SignedArea(qtc, tc2, tc3) / originSA;
	*beta = SignedArea(qtc, tc3, tc1) / originSA;
	*gamma = SignedArea(qtc, tc1, tc2) / originSA;

	qtc.x = *alpha*tc1.x + *beta*tc2.x + *gamma*tc3.x;
	qtc.y = *alpha*tc1.y + *beta*tc2.y + *gamma*tc3.y;

	return qtc;
}

double getDist(const float *p1,const float *p2){

	double dist=0;
	for (int i = 0; i < 3; i++){
		dist += ((p1[i] - p2[i])*(p1[i] - p2[i]));
	}

	return sqrt(dist);
}


void paintTriangle(unsigned char *img,int* color,int height,int width,PointCoord a, PointCoord b, PointCoord c){
	//Sort by y-coord
	PointCoord temp;
	if (a.y < b.y){
		temp = a;
		a = b;
		b = temp;
	}
	if (b.y < c.y){
		temp = b;
		b = c;
		c = temp;
	}
	if (a.y < b.y){
		temp = a;
		a = b;
		b = temp;
	}

	double slope_ac = (double)(a.y - c.y) / (a.x - c.x);//delta x=1/slope
	double slope_ab = (double)(a.y - b.y) / (a.x - b.x);
	double slope_bc = (double)(b.y - c.y) / (b.x - c.x);

	int start = a.x, end = a.x;
	//double start = a.x, end = a.x;
	double candi1 = a.x, candi2 = a.x;
	double slope_var = slope_ab;
	
	for (int i = a.y; i >= c.y; i--){
		if (i == b.y) slope_var = slope_bc;
		/*int j = start;
		do{
		printf("%d %d|", j, i);
		j++;
		} while (j < end);*/

		for (int j = start; j <= end; j++){
			//printf("%d %d|", j, i);
			int pixel = (j + i*width)*3;
			if (pixel >= height*width * 3)
				pixel = height*width * 3 - 1;
			if (pixel < 0) pixel = 0;

			img[pixel + 2] = (unsigned char)color[0];
			img[pixel + 1] = (unsigned char)color[1];
			img[pixel + 0] = (unsigned char)color[2];
		}
		//printf("\n");
		candi1 = candi1 - 1 / slope_ac;
		candi2 = candi2 - 1 / slope_var;
		start = floor(candi1);
		end = ceil(candi2);
		//start = int(candi1+0.5);
		//end = int(candi2+0.5);
		if (candi1 > candi2){
			//start = int(candi2+0.5);
			//end = int(candi1+0.5);
			start = floor(candi2);
			end = ceil(candi1);
		}
	}
}

void paintPicture(unsigned char *img,int width,int height){

	//extern PointMap points_data;

	FILE *fp = fopen("error.txt", "w");

	float max_dist = 10000.0;
	int npoints = 20;

	int i, len;

	len = tm.size();

	for (i = 0; i < len; i++){
		Texture tc1, tc2, tc3, qtc;
		Vertex qm, v1, v2, v3;
		Normal nm, n1, n2, n3;
		//double alpha, beta, gamma;

		NearestPoints np;

		tc1 = t[tm[i].p[0].textureidx - 1];
		tc2 = t[tm[i].p[1].textureidx - 1];
		tc3 = t[tm[i].p[2].textureidx - 1];
		PointCoord a, b, c;
		a.x = (int)(tc1.x*width + 0.5); a.y = (int)(tc1.y*height + 0.5);
		b.x = (int)(tc2.x*width + 0.5); b.y = (int)(tc2.y*height + 0.5);
		c.x = (int)(tc3.x*width + 0.5); c.y = (int)(tc3.y*height + 0.5);

		v1 = v[tm[i].p[0].vertexidx - 1];
		v2 = v[tm[i].p[1].vertexidx - 1];
		v3 = v[tm[i].p[2].vertexidx - 1];

		n1 = n[tm[i].p[0].normalidx - 1];
		n2 = n[tm[i].p[1].normalidx - 1];
		n3 = n[tm[i].p[2].normalidx - 1];

		/*if ((a == b) || (b == c) || (a == c)){
			if (a == b){
				np.pos[0] = v1.x;
				np.pos[1] = v1.y;
				np.pos[2] = v1.z;
			}
			else if (b == c){
				np.pos[0] = v2.x;
				np.pos[1] = v2.y;
				np.pos[2] = v2.z;
			}
			else{
				np.pos[0] = v1.x;
				np.pos[1] = v1.y;
				np.pos[2] = v1.z;
			}
		}*/
		
			//Calculate the barycentric point of three points on Texture Img(2nd dim)
			//qtc = getBarycentric(&alpha, &beta, &gamma, tc1, tc2, tc3);

			//Calculate the barycentric point of three points on PointCloud(3rd dim)
			qm.x = v1.x/3 + v2.x/3 + v3.x/3;
			qm.y = v1.y/3 + v2.y/3 + v3.y/3;
			qm.z = v1.z/3 + v2.z/3 + v3.z/3;

			/*nm.x = alpha*n1.x + beta*n2.x + gamma*n3.x;
			nm.y = alpha*n1.y + beta*n2.y + gamma*n3.y;
			nm.z = alpha*n1.z + beta*n2.z + gamma*n3.z;*/

			np.pos[0] = qm.x; np.pos[1] = qm.y; np.pos[2] = qm.z;
			
		
		//Find 10 Nearest Neighbors.
		np.dist2 = (float*)malloc(sizeof(float)*(npoints + 1));
		np.index = (const Point**)malloc(sizeof(Point*)*(npoints + 1));

		np.max = npoints;
		np.found = 0;
		np.got_heap = 0;
		np.dist2[0] = max_dist*max_dist;

		points_data->locate_points(&np, 1);

		//calculate color 
		int mix_color[3] = { 0 };
		double sum_dist=0,avr_dist=0;
		

		//Distance weighted 
		for (int i = 1; i <= np.found; i++){
		
			sum_dist += np.dist2[i];
		}
		avr_dist = 2*sum_dist / np.found;

		for (int i = 1; i <= np.found; i++){
			for (int j = 0; j < 3;j++)
				mix_color[j] += (int)(((avr_dist - np.dist2[i]) / sum_dist)*np.index[i]->color[j]);
		}

		//Just Average points' color
		/*for (int i = 1; i <= np.found; i++){
			mix_color[0] += np.index[i]->color[0];
			mix_color[1] += np.index[i]->color[1];
			mix_color[2] += np.index[i]->color[2];
		}
		
		if (np.found != 0){
			mix_color[0] /= np.found;
			mix_color[1] /= np.found;
			mix_color[2] /= np.found;
		}*/

		if (np.found==0){
			fprintf(fp, "Error Point : %d, %8.2lf %8.2lf %8.2lf/%8.2lf %8.2lf %8.2lf/%8.2lf %8.2lf %8.2lf \n", i, v1.x, v1.y, v1.z, v2.z, v2.y, v2.z, v3.x, v3.y, v3.z);
			fprintf(fp, "\t\tTexture point : %5lf %5lf/ %5lf %5lf/ %5lf %5lf\nnpfound:%d nppos %lf %lf %lf\n", a.x, a.y, b.x, b.y, c.x, c.y,np.found,np.pos[0],np.pos[1],np.pos[2]);
		}
		//paint color to img arr
		paintTriangle(img, mix_color, height, width, a, b, c);
		
		//free(dist);
	}
	fclose(fp);
}
