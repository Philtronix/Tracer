#include "Matrix4.h"
#include <math.h>
#include <cstdio>

Matrix4::Matrix4()
{
}

Matrix4::~Matrix4()
{
}

Matrix4::Matrix4(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D v4)
{
	r1 = v1;
	r2 = v2;
	r3 = v3;
	r4 = v4;
}

// a b c d     x   ax + by + cz + d
// e f g h  *  y = ex + fy + gz + h
// i j k l     z   ix + jy + kz + l
// 0 0 0 1     1         1
/*
Vec3D Matrix4::operator*(const Vec3D *m)
{
	Vec3D	result;

	result.x = (this->r1.x * m.x) + (this->r1.y * m.y) + (this->r1.z * m.z) + this->r1.w;
	result.y = (this->r2.x * m.x) + (this->r2.y * m.y) + (this->r2.z * m.z) + this->r2.w;
	result.z = (this->r3.x * m.x) + (this->r3.y * m.y) + (this->r3.z * m.z) + this->r3.w;
	result.x = 1;

	return result;
}
*/

#define PI         3.147

void Matrix4::World(double rotX,   double rotY,   double rotZ,
					double transX, double transY, double transZ,
					double scale)
{
	// Rotate
	double phi = (rotX * PI) / 180.0;
	double omg = (rotY * PI) / 180.0;
	double kap = (rotZ * PI) / 180.0;

	this->r1.x = cos(phi) * cos(kap);
	this->r1.y = cos(omg) * sin(kap) + sin(omg) * sin(phi) * cos(kap);
	this->r1.z = sin(omg) * sin(kap) - cos(omg) * sin(phi) * cos(kap);

	this->r2.x = -cos(phi) * sin(kap);
	this->r2.y =  cos(omg) * cos(kap) - sin(omg) * sin(phi) * sin(kap);
	this->r2.z =  sin(omg) * cos(kap) + cos(omg) * sin(phi) * sin(kap);

	this->r3.x =  sin(phi);
	this->r3.y = -sin(omg) * cos(phi);
	this->r3.z =  cos(omg) * cos(phi);

	// Scale
	// [x, 0, 0, 0]
	// [0, y, 0, 0]
	// [0, 0, z, 0]
	// [0, 0, 0, 1]

//	this->r1.x *= scale;
//	this->r2.y *= scale;
//	this->r3.z *= scale;

/*
matrix x matrix
[21, 22, 23, 24]   [ 5,  6,  7,  8]   [_00, _01, _02, _03]
[25, 26, 27, 28] x [ 9, 10, 11, 12] = [_10, _11, _12, _13]
[29, 30, 31, 32]   [13, 14, 15, 16]   [_20, _21, _22, _23]
[33, 34, 35, 36]   [17, 18, 19, 20]   [_30, _31, _32, _33]

Get the first row of the final matrix
Get the first element of this row in the final matrix:
                   [ 5]
[21, 22, 23, 24] . [ 9] = [21*5 + 22*9  + 23*13 + 24*17] = [105 + 198 + 299 + 408] =  1010 = _00
                   [13]
                   [17]

Get the second element of this row in the final matrix:
                   [ 6]
[21, 22, 23, 24] . [10] = [21*6 + 22*10 + 23*14 + 24*18] = [126 + 220 + 322 + 432] =  1100 = _01
                   [14]
                   [18]

Get the third element of this row in the final matrix:
                   [ 7]
[21, 22, 23, 24] . [11] = [21*7 + 22*11 + 23*15 + 24*19] = [147 + 242 + 345 + 456] =  1190 = _02
                   [15]
                   [19]

Get the fourth element of this row in the final matrix:
                   [ 8]
[21, 22, 23, 24] . [12] = [21*8 + 22*12 + 23*16 + 24*20] = [168 + 264 + 368 + 480] =  1280 = _03
                   [16]
                   [20]

first row of final matrix is:
[1010, 1100, 1190, 1280]

now lets get the second row of the final matrix
Get the first element of this row in the final matrix:
                   [ 5]
[25, 26, 27, 28] . [ 9] = [25*5 + 26*9  + 27*13 + 28*17] = [125 + 234 + 351 + 476] =  1186 = _00
                   [13]
                   [17]

Get the second element of this row in the final matrix:
                   [ 6]
[25, 26, 27, 28] . [10] = [25*6 + 26*10 + 27*14 + 28*18] = [150 + 260 + 378 + 504] =  1292 = _01
                   [14]
                   [18]

Get the third element of this row in the final matrix:
                   [ 7]
[25, 26, 27, 28] . [11] = [25*7 + 26*11 + 27*15 + 28*19] = [175 + 286 + 405 + 532] =  1398 = _02
                   [15]
                   [19]

Get the fourth element of this row in the final matrix:
                   [ 8]
[25, 26, 27, 28] . [12] = [25*8 + 26*12 + 27*16 + 28*20] = [200 + 312 + 432 + 560] =  1504 = _03
                   [16]
                   [20]

first row of final matrix is:
[1186, 1292, 1398, 1504]

I won't do the last two rows here, but i'll give you the final result so if you want to try yourself you can compare
[1010, 1100, 1190, 1280]
[1186, 1292, 1398, 1504]
[1362, 1484, 1606, 1728]
[1538, 1676, 1814, 1952]
*/

	// Translate
	// [1, 0, 0, x]
	// [0, 1, 0, y]
	// [0, 0, 1, z]
	// [0, 0, 0, 1]


}

// Pitch must be in the range of [-90 ... 90] degrees and 
// yaw must be in the range of [0 ... 360] degrees.
// Pitch and yaw variables must be expressed in radians.
void Matrix4::FPSViewRH(Vec3D eye, double pitch, double yaw)
{
	// I assume the values are already converted to radians.
	double cosPitch = cos(pitch);
	double sinPitch = sin(pitch);
	double cosYaw   = cos(yaw);
	double sinYaw   = sin(yaw);

	Vec3D xaxis(cosYaw, 0, -sinYaw);
	Vec3D yaxis(sinYaw * sinPitch,  cosPitch,   cosYaw * sinPitch);
	Vec3D zaxis(sinYaw * cosPitch, -sinPitch, cosPitch *   cosYaw );

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	this->r1.x = xaxis.x;
	this->r1.y = yaxis.x;
	this->r1.z = zaxis.x;
	this->r1.w = 0.0;

	this->r2.x = xaxis.y;
	this->r2.y = yaxis.y;
	this->r2.z = zaxis.y;
	this->r2.w = 0.0;

	this->r3.x = xaxis.z;
	this->r3.y = yaxis.z;
	this->r3.z = zaxis.z;
	this->r3.w = 0.0;

	// vec4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
	// TODO : dot correct is correct but eye has no effect
	this->r4.x = -xaxis.dot(eye);
	this->r4.y = -xaxis.dot(eye);
	this->r4.z = -xaxis.dot(eye);
	this->r4.w = 1.0;
}

Vec3D Matrix4::Vector_Normalise(Vec3D &v)
{
	float l = Vector_Length(v);
	return { v.x / l, v.y / l, v.z / l };
}

float Matrix4::Vector_DotProduct(Vec3D &v1, Vec3D &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

Vec3D Matrix4::Vector_CrossProduct(Vec3D &v1, Vec3D &v2)
{
	Vec3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

Vec3D Matrix4::Vector_Mul(Vec3D &v1, double k)
{
	return { v1.x * k, v1.y * k, v1.z * k };
}

Vec3D Matrix4::Vector_Sub(Vec3D &v1, Vec3D &v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

double Matrix4::Vector_Length(Vec3D &v)
{
	return sqrtf(Vector_DotProduct(v, v));
}

void Matrix4::LookAtRH(Vec3D eye, Vec3D target, Vec3D up)
{
/*
// Create a 4x4 orientation matrix from the right, up, and forward vectors
// This is transposed which is equivalent to performing an inverse
// if the matrix is orthonormalized (in this case, it is).
mat4 orientation = {
vec4( xaxis.x, yaxis.x, zaxis.x, 0 ),
vec4( xaxis.y, yaxis.y, zaxis.y, 0 ),
vec4( xaxis.z, yaxis.z, zaxis.z, 0 ),
vec4(   0,       0,       0,     1 )
};

// Create a 4x4 translation matrix.
// The eye position is negated which is equivalent
// to the inverse of the translation matrix.
// T(v)^-1 == T(-v)
mat4 translation = {
vec4(   1,      0,      0,   0 ),
vec4(   0,      1,      0,   0 ),
vec4(   0,      0,      1,   0 ),
vec4(-eye.x, -eye.y, -eye.z, 1 )
};

// Combine the orientation and translation to compute
// the final view matrix. Note that the order of
// multiplication is reversed because the matrices
// are already inverted.
return ( orientation * translation );
*/


//	mat4x4 Matrix_PointAt(vec3d &pos, vec3d &target, vec3d &up)
	{
		// Calculate new forward direction
//		Vec3D newForward = Vector_Sub(target, pos);
		Vec3D newForward = target - eye;
		newForward = Vector_Normalise(newForward);
/*
	vec3d Vector_Normalise(vec3d &v)
	{
		float l = Vector_Length(v);
		return { v.x / l, v.y / l, v.z / l };
	}

	float Vector_DotProduct(vec3d &v1, vec3d &v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
	}

	vec3d Vector_CrossProduct(vec3d &v1, vec3d &v2)
	{
		vec3d v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}
*/


		// Calculate new Up direction
		Vec3D a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
		Vec3D newUp = Vector_Sub(up, a);
		newUp = Vector_Normalise(newUp);

		// New Right direction is easy, its just cross product
		Vec3D newRight = Vector_CrossProduct(newUp, newForward);

		// Construct Dimensioning and Translation Matrix	
//		matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
		this->r1.x = newRight.x;
		this->r1.y = newRight.y;
		this->r1.z = newRight.z;
		this->r1.w = 0.0;

//		matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
		this->r2.x = newUp.x;
		this->r2.y = newUp.y;
		this->r2.z = newUp.z;
		this->r2.w = 0.0;

//		matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
		this->r3.x = newForward.x;
		this->r3.y = newForward.y;
		this->r3.z = newForward.z;
		this->r3.w = 0.0;

//		matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
		this->r4.x = eye.x;
		this->r4.y = eye.y;
		this->r4.z = eye.z;
		this->r4.w = 1.0;
	}

/* WAS THIS

	Vec3D zaxis;	// The "forward" vector.
	Vec3D xaxis;	// The "right" vector.
	Vec3D yaxis;	// The "up" vector.

	//vec3 zaxis = normal(eye - target);
	//vec3 xaxis = normal(cross(up, zaxis));
	//vec3 yaxis = cross(zaxis, xaxis);
	zaxis = eye - target;
	zaxis.normalise();

	xaxis.cross(up, zaxis);
	xaxis.normalise();

	yaxis.cross(zaxis, xaxis);

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	//mat4 viewMatrix = {
	//	Vec3D(xaxis.x,            yaxis.x,            zaxis.x,       0),
	//	Vec3D(xaxis.y,            yaxis.y,            zaxis.y,       0),
	//	Vec3D(xaxis.z,            yaxis.z,            zaxis.z,       0),
	//	Vec3D(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye),  1)};
	this->r1.x = xaxis.x;
	this->r1.y = yaxis.x;
	this->r1.z = zaxis.x;
	this->r1.w = 0;

	this->r2.x = xaxis.y;
	this->r2.y = yaxis.y;
	this->r2.z = zaxis.y;
	this->r2.w = 0;

	this->r3.x = xaxis.z;
	this->r3.y = yaxis.z;
	this->r3.z = zaxis.z;
	this->r3.w = 0;

	//	Vec3D(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye),  1)};
	this->r4.x = -Vec3D::dot(xaxis, eye);
	this->r4.y = -Vec3D::dot(yaxis, eye);
	this->r4.z = -Vec3D::dot(zaxis, eye);
	this->r4.w = 1;
*/
}


// EOF
