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
