#include "operation.h"

double SignedArea(const Texture& p, const Texture& q, const Texture& r){
	double result;
	Vector temp1, temp2, temp3;
	temp1=TextureSubstraction(q, p);
	temp2=TextureSubstraction(r, p);

	temp3=CrossProduct(temp1, temp2);

	result = sqrt(temp3.x*temp3.x + temp3.y*temp3.y);

	if (temp3.z != 1) result = (-result);

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
