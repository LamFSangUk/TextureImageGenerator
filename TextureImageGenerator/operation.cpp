#include "operation.h"

double SignedArea(const Texture& p, const Texture& q, const Texture& r){
	double result;
	Vector temp1, temp2, temp3;
	temp1=TextureSubstraction(q, p);
	temp2=TextureSubstraction(r, p);

	//printf("q:%lf, %lf\n", q.x, q.y);
	//printf("p:%lf, %lf\n", p.x, p.y);
	//printf("%lf, %lf\n", temp1.x, temp1.y);
	//printf("%lf, %lf\n", temp2.x, temp2.y);

	temp3=CrossProduct(temp1, temp2);

	/*printf("temp3 : %lf, %lf %lf\n\n", temp3.x, temp3.y,temp3.z);*/

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

	//printf("origin:%lf\n", originSA);

	qtc.x = (tc1.x + tc2.x + tc3.x) / 3;
	qtc.y = (tc1.y + tc2.y + tc3.y) / 3;

	*alpha = SignedArea(qtc, tc2, tc3) / originSA;
	*beta = SignedArea(qtc, tc3, tc1) / originSA;
	*gamma = SignedArea(qtc, tc1, tc2) / originSA;

	qtc.x = *alpha*tc1.x + *beta*tc2.x + *gamma*tc3.x;
	qtc.y = *alpha*tc1.y + *beta*tc2.y + *gamma*tc3.y;

	return qtc;
}

void paintPicture(unsigned char *img,int width,int height){

	extern PointMap points_data;

	float max_dist = 10000.0;
	int npoints = 10;

	int i, len;

	len = tm.size();
	//printf("%d\n", len);
	for (i = 0; i < len; i++){
		Texture tc1, tc2, tc3, qtc;
		Vertex qm, v1, v2, v3;
		Normal nm, n1, n2, n3;
		double alpha, beta, gamma;
		tc1 = t[tm[i].p[0].textureidx - 1];
		tc2 = t[tm[i].p[1].textureidx - 1];
		tc3 = t[tm[i].p[2].textureidx - 1];

		//printf("%lf %lf \n", tc1.x, tc1.y);

		v1 = v[tm[i].p[0].vertexidx - 1];
		v2 = v[tm[i].p[1].vertexidx - 1];
		v3 = v[tm[i].p[2].vertexidx - 1];

		n1 = n[tm[i].p[0].normalidx - 1];
		n2 = n[tm[i].p[1].normalidx - 1];
		n3 = n[tm[i].p[2].normalidx - 1];

		qtc = getBarycentric(&alpha, &beta, &gamma, tc1, tc2, tc3);

		//printf("%lf %lf %lf//\n", alpha, beta, gamma);

		qm.x = alpha*v1.x + beta*v2.x + gamma*v3.x;
		qm.y = alpha*v1.y + beta*v2.y + gamma*v3.y;
		qm.z = alpha*v1.z + beta*v2.z + gamma*v3.z;

		nm.x = alpha*n1.x + beta*n2.x + gamma*n3.x;
		nm.y = alpha*n1.y + beta*n2.y + gamma*n3.y;
		nm.z = alpha*n1.z + beta*n2.z + gamma*n3.z;

		NearestPoints np;

		np.dist2 = (float*)malloc(sizeof(float)*(npoints + 1));
		np.index = (const Point**)malloc(sizeof(Point*)*(npoints + 1));

		np.pos[0] = qm.x; np.pos[1] = qm.y; np.pos[2] = qm.z;
		np.max = npoints;
		np.found = 0;
		np.got_heap = 0;
		np.dist2[0] = max_dist*max_dist;

		points_data.locate_points(&np, 1);

		//check
		/*printf("point : %lf %lf %lf/%lf %lf %lf/%lf %lf %lf\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		printf("check : %d %lf %lf %lf\n", i, qm.x, qm.y, qm.z);
		printf("%d\n", np.found);*/

		int mix_color[3] = { 0 };

		for (int i = 1; i <= np.found; i++){
			//printf("%f %f %f %u %u %u\n", np.index[i]->pos[0], np.index[i]->pos[1], np.index[i]->pos[2], np.index[i]->color[0], np.index[i]->color[1], np.index[i]->color[2]);
			mix_color[0] += np.index[i]->color[0];
			mix_color[1] += np.index[i]->color[1];
			mix_color[2] += np.index[i]->color[2];
		}
		//printf("\n");

		//calculate color
		if (np.found != 0){
			mix_color[0] /= np.found;
			mix_color[1] /= np.found;
			mix_color[2] /= np.found;
		}

		////printf("%d %d\n", (int)(qtc.x*width), (int)(qtc.y*height));
		if ((int)(qtc.x*width) >= 0 && (int)(qtc.y*height) >= 0 && (int)(qtc.x*width) < width && (int)(qtc.y*height) < height){
			int pixel = ((int)(qtc.x*width) + (int)(qtc.y*height)*width) * 3;
			if (pixel >= height*width * 3)
				pixel = height*width * 3 - 1;
			if (pixel < 0) pixel = 0;
			img[pixel + 2] = (unsigned char)mix_color[0];
			img[pixel + 1] = (unsigned char)mix_color[1];
			img[pixel + 0] = (unsigned char)mix_color[2];
		}


	}
}