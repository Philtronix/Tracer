#pragma once

#include "Vec3D.h"


class Matrix4
{
public:
	Matrix4();
	~Matrix4();

	Matrix4(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D v4);

//	Vec3D operator*(const Vec3D *m);
//	Vec3D operator+(const Vec3D& m);
//	Matrix4 operator*(const Matrix4& m);
//	Matrix4 operator+(const Matrix4& m);

	void FPSViewRH(Vec3D eye, double pitch, double yaw);
	void LookAtRH(Vec3D eye, Vec3D target, Vec3D up);


	////////// NEW ///////////////
	Vec3D Vector_Normalise(Vec3D &v);
	float Vector_DotProduct(Vec3D &v1, Vec3D &v2);
	Vec3D Vector_CrossProduct(Vec3D &v1, Vec3D &v2);
	Vec3D Vector_Mul(Vec3D &v1, double k);
	Vec3D Vector_Sub(Vec3D &v1, Vec3D &v2);
	double Vector_Length(Vec3D &v);
	///////////////////////////////////////////

	Vec3D	r1;
	Vec3D	r2;
	Vec3D	r3;
	Vec3D	r4;
};

