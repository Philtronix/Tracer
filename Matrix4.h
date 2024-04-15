#pragma once

#include "Vec3D.h"
#include <stdint.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>

// https://www.scratchapixel.com/code.php?id=22&origin=/lessons/mathematics-physics-for-computer-graphics/geometry

class Matrix4
{
public:
	Matrix4();
	~Matrix4();

    double x[4][4] = {{1, 0, 0, 0},
					  {0, 1, 0, 0},
					  {0, 0, 1, 0},
					  {0, 0, 0, 1}};

	Matrix4 (double a, double b, double c, double d, 
			 double e, double f, double g, double h,
			 double i, double j, double k, double l,
			 double m, double n, double o, double p);

	const double* operator [] (uint8_t i) const 
	{
		return x[i];
	}

	double* operator [] (uint8_t i)
	{
		return x[i];
	}

	// Multiply the current matrix with another matrix (rhs)
	Matrix4 operator * (const Matrix4& v)
	{
		Matrix4 tmp;
		multiply (*this, v, tmp);

		return tmp;
	}

	// To make it easier to understand how a matrix multiplication works, the fragment of code
	// included within the #if-#else statement, show how this works if you were to iterate
	// over the coefficients of the resulting matrix (a). However you will often see this
	// multiplication being done using the code contained within the #else-#end statement.
	// It is exactly the same as the first fragment only we have litteraly written down
	// as a series of operations what would actually result from executing the two for() loops
	// contained in the first fragment. It is supposed to be faster, however considering
	// matrix multiplicatin is not necessarily that common, this is probably not super
	// useful nor really necessary (but nice to have -- and it gives you an example of how
	// it can be done, as this how you will this operation implemented in most libraries).
	void multiply(const Matrix4 &a, const Matrix4& b, Matrix4 &c);

	// Beturn a transposed copy of the current matrix as a new matrix
	Matrix4 transposed() const;

	// transpose itself
	Matrix4& transpose ();

    // This method needs to be used for point-matrix multiplication. Keep in mind
    // we don't make the distinction between points and vectors at least from
    // a programming point of view, as both (as well as normals) are declared as Vec3.
    // However, mathematically they need to be treated differently. Points can be translated
    // when translation for vectors is meaningless. Furthermore, points are implicitly
    // be considered as having homogeneous coordinates. Thus the w coordinates needs
    // to be computed and to convert the coordinates from homogeneous back to Cartesian
    // coordinates, we need to divided x, y z by w.
    //
    // The coordinate w is more often than not equals to 1, but it can be different than
    // 1 especially when the matrix is projective matrix (perspective projection matrix).
	void multVecMatrix(const Vec3D &src, Vec3D &dst) const;

	// This method needs to be used for vector-matrix multiplication. Look at the differences
	// with the previous method (to compute a point-matrix multiplication). We don't use
	// the coefficients in the matrix that account for translation (x[3][0], x[3][1], x[3][2])
	// and we don't compute w.
	void multDirMatrix(const Vec3D &src, Vec3D &dst);

	// Compute the inverse of the matrix using the Gauss-Jordan (or reduced row) elimination method.
	// We didn't explain in the lesson on Geometry how the inverse of matrix can be found. Don't
	// worry at this point if you don't understand how this works. But we will need to be able to
	// compute the inverse of matrices in the first lessons of the "Foundation of 3D Rendering" section,
	// which is why we've added this code. For now, you can just use it and rely on it
	// for doing what it's supposed to do. If you want to learn how this works though, check the lesson
	// on called Matrix Inverse in the "Mathematics and Physics of Computer Graphics" section.
	Matrix4 inverse() const;

	// Set current matrix to its inverse
	Matrix4& invert();

/*
	friend std::ostream& operator << (std::ostream &s, const Matrix4 &m)
	{
		std::ios_base::fmtflags oldFlags = s.flags();
		int width = 12; // total with of the displayed number
		s.precision(5); // control the number of displayed decimals
		s.setf (std::ios_base::fixed);
		
		s << "[" << std::setw (width) << m[0][0] <<
				" " << std::setw (width) << m[0][1] <<
				" " << std::setw (width) << m[0][2] <<
				" " << std::setw (width) << m[0][3] << "\n" <<
			
				" " << std::setw (width) << m[1][0] <<
				" " << std::setw (width) << m[1][1] <<
				" " << std::setw (width) << m[1][2] <<
				" " << std::setw (width) << m[1][3] << "\n" <<
			
				" " << std::setw (width) << m[2][0] <<
				" " << std::setw (width) << m[2][1] <<
				" " << std::setw (width) << m[2][2] <<
				" " << std::setw (width) << m[2][3] << "\n" <<
			
				" " << std::setw (width) << m[3][0] <<
				" " << std::setw (width) << m[3][1] <<
				" " << std::setw (width) << m[3][2] <<
				" " << std::setw (width) << m[3][3] << "]";
		
		s.flags (oldFlags);
		return s;
	}
*/

///////////////////////////////////////////////////////////////////////////////

	void World(double rotX, double rotY, double rotZ);
	void Scale(double sX, double sY, double sZ);
	void Translate(double tX, double tY, double tZ);
	void lookAt(Vec3D from, Vec3D to, Vec3D tmp = Vec3D(0.0, 1.0, 0.0));
	void perspective();
	void orthographic();
	void FPSViewRH(Vec3D eye, double pitch, double yaw);
	void LookAtRH(Vec3D eye, Vec3D target, Vec3D up);

	void dump(const char *text);

	Vec3D crossProduct(Vec3D a, Vec3D b);
	Vec3D normalise(Vec3D a);

	void ProjectionMatrix(const double near_plane, // Distance to near clipping plane
                          const double far_plane,  // Distance to far clipping plane
                          const double fov_horiz,  // Horizontal field of view angle, in radians
                          const double fov_vert);  // Vertical field of view angle, in radians

	void BuildPerspProjMat(double fov, double aspect, double znear, double zfar);
};

