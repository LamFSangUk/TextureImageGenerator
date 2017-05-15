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

double DotProduct(Normal a, const double *b){
	double ret = a.x*b[0] + a.y*b[1] + a.z*b[2];
	//if (ret < 0) ret *= -1;
	return ret;
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


void paintTriangle(unsigned char *img,bool*imgflag,int* color,int height,int width,PointCoord a, PointCoord b, PointCoord c){
	

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

			if (i>=0 && i<height && j>=0 && j<width && !imgflag[j+i*width]){
				img[pixel + 2] = (unsigned char)color[0];
				img[pixel + 1] = (unsigned char)color[1];
				img[pixel + 0] = (unsigned char)color[2];
				imgflag[j+i*width] = true;//ADDED 5.1.
			}
		}
		//printf("\n");
		candi1 = candi1 - 1 / slope_ac;
		candi2 = candi2 - 1 / slope_var;
		//start = floor(candi1);
		//if (candi1 - start >= 0.9) start += 1;
		//end = ceil(candi2);
		//if (end - candi2 >= 0.9) end -= 1;*/
		//start = int(candi1-1);
		//end = int(candi2+1);
		start = int(candi1 + 0.5);
		end = ceil(candi2);

		if (candi1 > candi2){
			//start = int(candi2-1);
			//end = int(candi1+1);
			///start = floor(candi2);
			//if (candi2 - start >= 0.9) start += 1;
			start = int(candi2 + 0.5);
			end = ceil(candi1);
			//if (end - candi1 >= 0.9) end -= 1;*/
		}
	}
}

void paintPicture(unsigned char *img,bool* imgflag,int width,int height){

	//extern PointMap points_data;

	FILE *fp = fopen("error.txt", "w");

	float max_dist = 100.0;
	int npoints = 100;

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

		//Sort by x-coord
		Texture temp;
		PointCoord a, b, c,tempc;
		if (tc1.x < tc2.x){
			temp = tc1;
			tc1 = tc2;
			tc2 = temp;
		}
		if (tc2.x < tc3.x){
			temp = tc2;
			tc2 = tc3;
			tc3 = temp;
		}
		if (tc1.x < tc2.x){
			temp = tc1;
			tc1 = tc2;
			tc2 = temp;
		}
		a.x = ceil(tc1.x*width);
		b.x = int(tc2.x*width + 0.5);
		c.x = floor(tc3.x*width);

		//Sort by y-coord
		if (tc1.y < tc2.y){
			temp = tc1;
			tc1 = tc2;
			tc2 = temp;
			
			tempc = a;
			a = b;
			b = tempc;
		}
		if (tc2.y < tc3.y){
			temp = tc2;
			tc2 = tc3;
			tc3 = temp;

			tempc = b;
			b = c;
			c = tempc;
		}
		if (tc1.y < tc2.y){
			temp = tc1;
			tc1 = tc2;
			tc2 = temp;

			tempc = a;
			a = b;
			b = tempc;
		}
		
		a.y = ceil(tc1.y*height);
		b.y = int(tc2.y*height+0.5);
		c.y = floor(tc3.y*height);

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

			nm.x = (n1.x + n2.x + n3.x) / 3;
			nm.y = (n1.y + n2.y + n3.y) / 3;
			nm.z = (n1.z + n2.z + n3.z) / 3;
			//normalize
			double mag = nm.x*nm.x + nm.y*nm.y + nm.z*nm.z;
			nm.x /= mag; nm.y /= mag; nm.z /= mag;

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
		float sum_color[3] = { 0 };
		int mix_color[3] = { 0 };
		double sum_dist=0,avr_dist=0;
		double sum_dot = 0,avr_dot=0;

		//Distance weighted 
		for (int i = 1; i <= np.found; i++){
		
			sum_dist += np.dist2[i];
			sum_dot += DotProduct(nm, np.index[i]->normal);
		}
		avr_dist = 2*sum_dist / np.found;
		avr_dot = 2*sum_dot / np.found;

		for (int i = 1; i <= np.found; i++){
			for (int j = 0; j < 3;j++)
				sum_color[j] += ((avr_dist - np.dist2[i]) / sum_dist)
								*(DotProduct(nm, np.index[i]->normal))*(DotProduct(nm, np.index[i]->normal))
								//((avr_dot-DotProduct(nm, np.index[i]->normal))/sum_dot)
								* np.index[i]->color[j];
		}
		
		for (int j = 0; j < 3; j++) mix_color[j] = (int)sum_color[j];

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
		paintTriangle(img, imgflag, mix_color, height, width, a, b, c);
		
		//free(dist);
		free(np.dist2);
		free(np.index);
	}
	fclose(fp);
}
