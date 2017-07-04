#include "operation.h"

double DotProduct(Normal a, const double *b){
	double ret = a.x*b[0] + a.y*b[1] + a.z*b[2];
	//if (ret < 0) ret *= -1;
	return ret;
}


void paintTriangle(unsigned char *img,bool*imgflag,int* color,int height,int width,PointCoord a, PointCoord b, PointCoord c){
	

	double slope_ac = (double)(a.y - c.y) / (a.x - c.x);//delta x=1/slope
	double slope_ab = (double)(a.y - b.y) / (a.x - b.x);
	double slope_bc = (double)(b.y - c.y) / (b.x - c.x);

	int start = a.x, end = a.x;
	double candi1 = a.x, candi2 = a.x;
	double slope_var = slope_ab;
	
	for (int i = a.y; i >= c.y; i--){
		if (i == b.y) slope_var = slope_bc;

		for (int j = start; j <= end; j++){
			
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

		candi1 = candi1 - 1 / slope_ac;
		candi2 = candi2 - 1 / slope_var;

		start = int(candi1 + 0.5);
		end = ceil(candi2);

		if (candi1 > candi2){

			start = int(candi2 + 0.5);
			end = ceil(candi1);
		}
	}
}

void paintPicture(unsigned char *img,bool* imgflag,int width,int height){

	FILE *fp = fopen("error.txt", "w");

	int i, len;
	float max_dist = 100.0;
	int npoints = 100;

	len = tm.size();

	for (i = 0; i < len; i++){
		Texture tc1, tc2, tc3, qtc;
		Vertex qm, v1, v2, v3;
		Normal nm, n1, n2, n3;

		NearestPoints np;

		tc1 = t[tm[i].p[0].textureidx - 1];
		tc2 = t[tm[i].p[1].textureidx - 1];
		tc3 = t[tm[i].p[2].textureidx - 1];

		v1 = v[tm[i].p[0].vertexidx - 1];
		v2 = v[tm[i].p[1].vertexidx - 1];
		v3 = v[tm[i].p[2].vertexidx - 1];

		n1 = n[tm[i].p[0].normalidx - 1];
		n2 = n[tm[i].p[1].normalidx - 1];
		n3 = n[tm[i].p[2].normalidx - 1];

		//Sort by x-coord
		Texture temp;
		PointCoord a, b, c, tempc;
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


		//Calculate the barycentric point of three points on PointCloud(3rd dim)
		qm.x = v1.x/3 + v2.x/3 + v3.x/3;
		qm.y = v1.y/3 + v2.y/3 + v3.y/3;
		qm.z = v1.z/3 + v2.z/3 + v3.z/3;

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
								* np.index[i]->color[j];
		}
		
		for (int j = 0; j < 3; j++) mix_color[j] = (int)sum_color[j];

		if (np.found==0){//For Debug
			fprintf(fp, "Error Point : %d, %8.2lf %8.2lf %8.2lf/%8.2lf %8.2lf %8.2lf/%8.2lf %8.2lf %8.2lf \n", i, v1.x, v1.y, v1.z, v2.z, v2.y, v2.z, v3.x, v3.y, v3.z);
			fprintf(fp, "\t\tTexture point : %5lf %5lf/ %5lf %5lf/ %5lf %5lf\nnpfound:%d nppos %lf %lf %lf\n", a.x, a.y, b.x, b.y, c.x, c.y,np.found,np.pos[0],np.pos[1],np.pos[2]);
		}

		//paint color to img arr
		paintTriangle(img, imgflag, mix_color, height, width, a, b, c);
		
		free(np.dist2);
		free(np.index);
	}
	fclose(fp);
}
