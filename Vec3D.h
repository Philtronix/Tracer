#pragma once

class Vec3D
{
public:
	Vec3D();
	~Vec3D();

	Vec3D(double pX, double pY, double pZ, double pW);
	Vec3D(double pX, double pY, double pZ);

	void RotX(double a);
	void RotY(double a);
	void RotZ(double a);
	void Translate(double tX, double tY, double tZ);
	void Scale(double sX, double sY, double sZ);

	static double dot(Vec3D a, Vec3D b);
	Vec3D cross(Vec3D a, Vec3D b);
	void normalise();

	Vec3D operator=(Vec3D *m);
	double operator*(Vec3D m);
	Vec3D operator-(Vec3D *m);
	Vec3D operator-(Vec3D m);
	Vec3D operator+(Vec3D *m);
	Vec3D operator+(Vec3D m);

	double	x;
	double	y;
	double	z;
	double	w;

//	double	nx;
//	double	ny;
//	double	nz;
};

