#include "Vec3D.h"
#include <math.h>

Vec3D::Vec3D()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vec3D::~Vec3D()
{
}

Vec3D::Vec3D(double pX, double pY, double pZ, double pW)
{
	x = pX;
	y = pY;
	z = pZ;
	w = pW;
}

Vec3D::Vec3D(double pX, double pY, double pZ)
{
	x = pX;
	y = pY;
	z = pZ;
	w = 0;
}

// a b c d     x   ax + by + cz + d
// e f g h  *  y = ex + fy + gz + h
// i j k l     z   ix + jy + kz + l
// 0 0 0 1     1         1

// 1  0     0   0
// 0 cosA -sinA 0
// 0 sinA cosA  0
// 0  0     0   1
void Vec3D::RotX(double a)
{
	double cosA = cos(a);
	double sinA = sin(a);
	double x = this->x;
	double y = this->y;
	double z = this->z;

	this->x = x;						// ax + by + cz + d
	this->y = (cosA * y) + (-sinA * z);	// ex + fy + gz + h
	this->z = (sinA * y) + (cosA * z);	// ix + jy + kz + l
	this->w = 1;						// 1
}

//  cosA   0   sinA   0
//    0    1     0    0
// -sinA   0   cosA   0
//    0    0     0    1
void Vec3D::RotY(double a)
{
	double cosA = cos(a);
	double sinA = sin(a);
	double x = this->x;
	double y = this->y;
	double z = this->z;

	this->x = (cosA * x) + (sinA * z);	// ax + by + cz + d
	this->y = y;						// ex + fy + gz + h
	this->z = (-sinA * x) + (cosA * z);	// ix + jy + kz + l
	this->w = 1;						// 1
}

// cosA  -sinA  0  0
// sinA   cosA  0  0
//   0     0    1  0
//   0     0    0  1
void Vec3D::RotZ(double a)
{
	double cosA = cos(a);
	double sinA = sin(a);
	double x = this->x;
	double y = this->y;
	double z = this->z;

	this->x = (cosA * x) + (-sinA * y);	// ax + by + cz + d
	this->y = (sinA * x) + (cosA * y);	// ex + fy + gz + h
	this->z = z;						// ix + jy + kz + l
	this->w = 1;						// 1
}

//  1  0  0  Tx
//  0  1  0  Ty
//  0  0  1  Tz
//  0  0  0  1
void Vec3D::Translate(double tX, double tY, double tZ)
{
	double x = this->x;
	double y = this->y;
	double z = this->z;

	this->x = x + tX;	// ax + by + cz + d
	this->y = y + tY;	// ex + fy + gz + h
	this->z = z + tZ;	// ix + jy + kz + l
	this->w = 1;		// 1
}

//  Sx  0   0   0
//  0   Sy  0   0
//  0   0   Sz  0
//  0   0   0   1
void Vec3D::Scale(double sX, double sY, double sZ)
{
	double x = this->x;
	double y = this->y;
	double z = this->z;

	this->x = x + (sX * x);		// ax + by + cz + d
	this->y = y + (sY * y);		// ex + fy + gz + h
	this->z = z + (sZ * z);		// ix + jy + kz + l
	this->w = 1;				// 1
}

Vec3D Vec3D::operator=(Vec3D *m)
{
	Vec3D result;
	this->x = m->x;
	this->y = m->y;
	this->z = m->z;
	this->w = m->w;

	return result;
}

Vec3D Vec3D::operator-(Vec3D *m)
{
	Vec3D result;
	result.x = this->x - m->x;
	result.y = this->y - m->y;
	result.z = this->z - m->z;

	return result;
}

Vec3D Vec3D::operator-(Vec3D m)
{
	Vec3D result;
	result.x = this->x - m.x;
	result.y = this->y - m.y;
	result.z = this->z - m.z;

	return result;
}

Vec3D Vec3D::operator+(Vec3D *m)
{
	Vec3D result;
	result.x = this->x + m->x;
	result.y = this->y + m->y;
	result.z = this->z + m->z;

	return result;
}

Vec3D Vec3D::operator+(Vec3D m)
{
	Vec3D result;
	result.x = this->x + m.x;
	result.y = this->y + m.y;
	result.z = this->z + m.z;

	return result;
}

double Vec3D::operator*(Vec3D m)
{
	double result;

	result = (this->x * m.x) + (this->y * m.y) + (this->z * m.z);

	return result;
}

double Vec3D::dot(Vec3D a, Vec3D b)
{
	double result;

	result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

	return result;
}

Vec3D Vec3D::cross(Vec3D a, Vec3D b)
{
	Vec3D result;
	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	result.w = 0;

	return result;
}

void Vec3D::normalise()
{
	double len = sqrt((x * x) + (y * y) + (z * z));

	x = x / len;
	y = y / len;
	z = z / len;
}


// EOF