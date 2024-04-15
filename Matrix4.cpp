#include "Matrix4.h"
#include <math.h>
#include <cstdio>

extern int ScreenWidth;
extern int ScreenHeight;

Matrix4::Matrix4()
{
}

Matrix4::~Matrix4()
{
}

Matrix4::Matrix4(double a, double b, double c, double d, 
				 double e, double f, double g, double h,
				 double i, double j, double k, double l, 
				 double m, double n, double o, double p)
{
	x[0][0] = a;
	x[0][1] = b;
	x[0][2] = c;
	x[0][3] = d;
	x[1][0] = e;
	x[1][1] = f;
	x[1][2] = g;
	x[1][3] = h;
	x[2][0] = i;
	x[2][1] = j;
	x[2][2] = k;
	x[2][3] = l;
	x[3][0] = m;
	x[3][1] = n;
	x[3][2] = o;
	x[3][3] = p;
}

void Matrix4::dump(const char *text)
{
	printf("-------------------\r\n%s\r\n", text);
	printf("%.2f %.2f %.2f %.2f\r\n", x[0][0], x[0][1], x[0][2], x[0][3]);
	printf("%.2f %.2f %.2f %.2f\r\n", x[1][0], x[1][1], x[1][2], x[1][3]);
	printf("%.2f %.2f %.2f %.2f\r\n", x[2][0], x[2][1], x[2][2], x[2][3]);
	printf("%.2f %.2f %.2f %.2f\r\n", x[3][0], x[3][1], x[3][2], x[3][3]);
	printf("===================\r\n");
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
void Matrix4::multiply(const Matrix4 &a, const Matrix4& b, Matrix4 &c)
{
	// A restric qualified pointer (or reference) is basically a promise
	// to the compiler that for the scope of the pointer, the target of the
	// pointer will only be accessed through that pointer (and pointers
	// copied from it.
	const double * __restrict ap = &a.x[0][0];
	const double * __restrict bp = &b.x[0][0];
	double       * __restrict cp = &c.x[0][0];
	
	double a0;
	double a1;
	double a2;
	double a3;

	a0 = ap[0];
	a1 = ap[1];
	a2 = ap[2];
	a3 = ap[3];

	cp[0] = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
	cp[1] = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
	cp[2] = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
	cp[3] = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];

	a0 = ap[4];
	a1 = ap[5];
	a2 = ap[6];
	a3 = ap[7];

	cp[4] = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
	cp[5] = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
	cp[6] = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
	cp[7] = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];

	a0 = ap[8];
	a1 = ap[9];
	a2 = ap[10];
	a3 = ap[11];

	cp[8]  = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
	cp[9]  = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
	cp[10] = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
	cp[11] = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];

	a0 = ap[12];
	a1 = ap[13];
	a2 = ap[14];
	a3 = ap[15];

	cp[12] = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
	cp[13] = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
	cp[14] = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
	cp[15] = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];
}

// Beturn a transposed copy of the current matrix as a new matrix
Matrix4 Matrix4::transposed() const
{
	return Matrix4 (x[0][0],
					x[1][0],
					x[2][0],
					x[3][0],
					x[0][1],
					x[1][1],
					x[2][1],
					x[3][1],
					x[0][2],
					x[1][2],
					x[2][2],
					x[3][2],
					x[0][3],
					x[1][3],
					x[2][3],
					x[3][3]);
}

// \brief transpose itself
Matrix4& Matrix4::transpose ()
{
	Matrix4 tmp (x[0][0],
				 x[1][0],
				 x[2][0],
				 x[3][0],
				 x[0][1],
				 x[1][1],
				 x[2][1],
				 x[3][1],
				 x[0][2],
				 x[1][2],
				 x[2][2],
				 x[3][2],
				 x[0][3],
				 x[1][3],
				 x[2][3],
			 	x[3][3]);
	*this = tmp;

	return *this;
}

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
void Matrix4::multVecMatrix(const Vec3D &src, Vec3D &dst) const
{
	double a;
	double b;
	double c;
	double w;

	a = src.x * x[0][0] + src.y * x[1][0] + src.z * x[2][0] + x[3][0];
	b = src.x * x[0][1] + src.y * x[1][1] + src.z * x[2][1] + x[3][1];
	c = src.x * x[0][2] + src.y * x[1][2] + src.z * x[2][2] + x[3][2];
	w = src.x * x[0][3] + src.y * x[1][3] + src.z * x[2][3] + x[3][3];
	
	dst.x = a / w;
	dst.y = b / w;
	dst.z = c / w;
}

// This method needs to be used for vector-matrix multiplication. Look at the differences
// with the previous method (to compute a point-matrix multiplication). We don't use
// the coefficients in the matrix that account for translation (x[3][0], x[3][1], x[3][2])
// and we don't compute w.
void Matrix4::multDirMatrix(const Vec3D &src, Vec3D &dst)
{
	double a;
	double b;
	double c;

	a = src.x * x[0][0] + src.y * x[1][0] + src.z * x[2][0];
	b = src.x * x[0][1] + src.y * x[1][1] + src.z * x[2][1];
	c = src.x * x[0][2] + src.y * x[1][2] + src.z * x[2][2];
	
	dst.x = a;
	dst.y = b;
	dst.z = c;
}

//[comment]
// Compute the inverse of the matrix using the Gauss-Jordan (or reduced row) elimination method.
// We didn't explain in the lesson on Geometry how the inverse of matrix can be found. Don't
// worry at this point if you don't understand how this works. But we will need to be able to
// compute the inverse of matrices in the first lessons of the "Foundation of 3D Rendering" section,
// which is why we've added this code. For now, you can just use it and rely on it
// for doing what it's supposed to do. If you want to learn how this works though, check the lesson
// on called Matrix Inverse in the "Mathematics and Physics of Computer Graphics" section.
//[/comment]
Matrix4 Matrix4::inverse() const
{
	int i, j, k;
	Matrix4 s;
	Matrix4 t (*this);
	
	// Forward elimination
	for (i = 0; i < 3 ; i++) 
	{
		int pivot = i;
		
		double pivotsize = t[i][i];
		
		if (pivotsize < 0)
		{
			pivotsize = -pivotsize;
		}

		for (j = i + 1; j < 4; j++) 
		{
			double tmp = t[j][i];
			
			if (tmp < 0)
			{
				tmp = -tmp;
			}
				
			if (tmp > pivotsize) 
			{
				pivot = j;
				pivotsize = tmp;
			}
		}

		if (pivotsize == 0) 
		{
			// Cannot invert singular matrix
			return Matrix4();
		}

		if (pivot != i) 
		{
			for (j = 0; j < 4; j++) 
			{
				double tmp;
				
				tmp = t[i][j];
				t[i][j] = t[pivot][j];
				t[pivot][j] = tmp;
				
				tmp = s[i][j];
				s[i][j] = s[pivot][j];
				s[pivot][j] = tmp;
			}
		}

		for (j = i + 1; j < 4; j++)
		{
			double f = t[j][i] / t[i][i];
			
			for (k = 0; k < 4; k++) 
			{
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}

	// Backward substitution
	for (i = 3; i >= 0; --i) 
	{
		double f;
		
		if ((f = t[i][i]) == 0) 
		{
			// Cannot invert singular matrix
			return Matrix4();
		}
		
		for (j = 0; j < 4; j++) 
		{
			t[i][j] /= f;
			s[i][j] /= f;
		}
		
		for (j = 0; j < i; j++) 
		{
			f = t[j][i];
			
			for (k = 0; k < 4; k++) 
			{
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}
	
	return s;
}

// Set current matrix to its inverse
Matrix4& Matrix4::invert()
{
	*this = inverse();
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

#define PI         3.147

void Matrix4::World(double rotX,   double rotY,   double rotZ)
{
	// Rotate
	double phi = (rotX * PI) / 180.0;
	double omg = (rotY * PI) / 180.0;
	double kap = (rotZ * PI) / 180.0;

	x[0][0] = cos(phi) * cos(kap);
	x[0][1] = cos(omg) * sin(kap) + sin(omg) * sin(phi) * cos(kap);
	x[0][2] = sin(omg) * sin(kap) - cos(omg) * sin(phi) * cos(kap);

	x[1][0] = -cos(phi) * sin(kap);
	x[1][1] =  cos(omg) * cos(kap) - sin(omg) * sin(phi) * sin(kap);
	x[1][2] =  sin(omg) * cos(kap) + cos(omg) * sin(phi) * sin(kap);

	x[2][0] =  sin(phi);
	x[2][1] = -sin(omg) * cos(phi);
	x[2][2] =  cos(omg) * cos(phi);
}

void Matrix4::Scale(double sX, double sY, double sZ)
{
	// Scale
	// [X, 0, 0, 0]
	// [0, Y, 0, 0]
	// [0, 0, Z, 0]
	// [0, 0, 0, 1]
	x[0][0] = sX;
	x[1][1] = sY;
	x[2][2] = sZ;

	x[3][3] = 1.0;
}

void Matrix4::Translate(double tX, double tY, double tZ)
{
	// Translate
	// [1, 0, 0, x]
	// [0, 1, 0, y]
	// [0, 0, 1, z]
	// [0, 0, 0, 1]
	//C  R
	x[0][0] = 1.0;
	x[3][0] = tX;

	x[1][1] = 1.0;
	x[3][1] = tY;

	x[2][2] = 1.0;
	x[3][2] = tZ;

	x[3][3] = 1.0;
}

//Step 1: compute the forward axis
//		Vec3f forward = Normalize(to - from);
//Step 2: compute the right vector
//		Vec3f right = crossProduct(randomVec, forward);
//		Vec3f tmp(0, 1, 0); 
//		Vec3f right = crossProduct(Normalize(tmp), forward); 
//Step 3: compute the up vector
//		Vec3f up = crossProduct(forward, right)
//Step 4: set the 4x4 matrix using the right, up and forward vector as from point.
void Matrix4::lookAt(Vec3D from, Vec3D to, Vec3D tmp)
{ 
	// rX rY rZ 0  : Right
	// uX uY uZ 0  : Up
	// fX fY fZ 0  : Forward
	// tX tY tZ 1  : Translation
    Vec3D forward = normalise(to - from); 
    Vec3D right   = crossProduct(normalise(tmp), forward); 
    Vec3D up      = crossProduct(forward, right); 
 
	//C  R
    x[0][0] = right.x; 
    x[0][1] = right.y; 
    x[0][2] = right.z; 
    x[0][3] = 0.0;

    x[1][0] = up.x; 
    x[1][1] = up.y; 
    x[1][2] = up.z;
    x[1][3] = 0.0;

    x[2][0] = forward.x; 
    x[2][1] = forward.y; 
    x[2][2] = forward.z; 
    x[2][3] = 0.0;
 
    x[3][0] = from.x; 
    x[3][1] = from.y; 
    x[3][2] = from.z; 
    x[3][3] = 1.0;

/*
	//C  R
    x[0][0] = right.x; 
    x[1][0] = right.y; 
    x[2][0] = right.z;
    x[3][0] = 0.0;

    x[0][1] = up.x; 
    x[1][1] = up.y; 
    x[2][1] = up.z;
    x[3][1] = 0.0;

    x[0][2] = forward.x; 
    x[1][2] = forward.y; 
    x[2][2] = forward.z; 
    x[3][2] = 0.0;
 
    x[0][3] = from.x; 
    x[1][3] = from.y; 
    x[2][3] = from.z; 
    x[3][3] = 1.0;
*/
}

// https://www.braynzarsoft.net/viewtutorial/q16390-transformations-and-world-view-projection-space-matrices

void Matrix4::orthographic()
{ 
/*
Orthographic Projection
w = 2/width
h = 2/height
a = 1.0f / (FarZ-NearZ)
b = -a * NearZ

orthographic projection matrix
[w, 0, 0, 0]
[0, h, 0, 0]
[0, 0, a, 0]
[0, 0, b, 1]
*/
//	double fovy  =    90.0;
	double zfar  = -1000.0;
	double znear =     1.0;

	double w = 2 / (double) ScreenWidth;
	double h = 2 / (double) ScreenHeight;
	double a = 1.0 / (zfar-znear);
	double b = -a * znear;

	//C  R
/*
    x[0][0] = w; 
    x[1][1] = h; 
    x[2][2] = a; 
    x[3][2] = 1.0;
    x[2][3] = b;
*/

    x[0][0] = w; 
    x[1][1] = h; 
    x[2][2] = a; 
    x[2][3] = 1.0;
    x[3][2] = b; 
}

void Matrix4::perspective()
{ 
/*
perspective projection
aspectRatio = width/height
h = 1 / tan(fovy*0.5)
w = h / aspectRatio
a = zfar / (zfar - znear)
b = (-znear * zfar) / (zfar - znear)

perspective projection matrix
   0  1  2  3
0 [w, 0, 0, 0]
1 [0, h, 0, 0]
2 [0, 0, a, 1]
3 [0, 0, b, 0]
*/
	double fovy  =    90.0;
	double zfar  = -1000.0;
	double znear =     1.0;

	double aspectRatio = ((double)ScreenWidth) / ((double)ScreenHeight);
	double h = 1 / tan(fovy*0.5);
	double w = h / aspectRatio;
	double a = zfar / (zfar - znear);
	double b = (-znear * zfar) / (zfar - znear);

	//C  R
/*
    x[0][0] = w; 
    x[1][1] = h; 
    x[2][2] = a; 
    x[3][2] = 1.0;
    x[2][3] = b;
*/

    x[0][0] = w; 
    x[1][1] = h; 
    x[2][2] = a; 
    x[2][3] = 1.0;
    x[3][2] = b; 
}
 
void Matrix4::ProjectionMatrix(const double near_plane, // Distance to near clipping plane
                               const double far_plane,  // Distance to far clipping plane
                               const double fov_horiz,  // Horizontal field of view angle, in radians
                               const double fov_vert)   // Vertical field of view angle, in radians
{
    double    h, w, Q;

    w = (double)1.0/tan(fov_horiz*0.5);  // 1/tan(x) == cot(x)
    h = (double)1.0/tan(fov_vert*0.5);   // 1/tan(x) == cot(x)
    Q = far_plane/(far_plane - near_plane);

    x[0][0] =  w;
    x[1][1] =  h;
    x[2][2] =  Q;
//    x[3][2] = -Q*near_plane;
//    x[2][3] =  1;
    x[2][3] = -Q*near_plane;
    x[3][2] =  1;
}



const double PI_OVER_360 = 0.0087266;

// https://www.geeks3d.com/20090729/howto-perspective-projection-matrix-in-opengl/
void Matrix4::BuildPerspProjMat(double fov, double aspect, double znear, double zfar)
{
	double xymax = znear * tan(fov * PI_OVER_360);
	double ymin  = -xymax;
	double xmin  = -xymax;

	double width  = xymax - xmin;
	double height = xymax - ymin;

	double depth = zfar - znear;
	double     q = -(zfar + znear) / depth;
	double    qn = -2 * (zfar * znear) / depth;

	double w = 2 * znear / width;
			w = w / aspect;
	double h = 2 * znear / height;

/*
  x[0][0] = w;
  x[1][0] = 0;
  x[2][0] = 0;
  x[3][0] = 0;

  x[0][1] = 0;
  x[1][1] = h;
  x[2][1] = 0;
  x[3][1] = 0;

  x[0][2] = 0;
  x[1][2] = 0;
  x[2][2] = q;
  x[3][2] = -1;

  x[0][3] = 0;
  x[1][3] = 0;
  x[2][3] = qn;
  x[3][3] = 1.0;
*/
	x[0][0] = w;
	x[0][1] = 0;
	x[0][2] = 0;
	x[0][3] = 0;

	x[1][0] = 0;
	x[1][1] = h;
	x[1][2] = 0;
	x[1][3] = 0;

	x[2][0] = 0;
	x[2][1] = 0;
	x[2][2] = q;
//  x[2][3] = -1;		// ?????

	x[3][0] = 0;
	x[3][1] = 0;
	x[3][2] = qn;
	x[3][3] = 1.0;
}

///////////////////////////////////////////////////////////////////////////////

// https://www.3dgep.com/understanding-the-view-matrix/

// Pitch must be in the range of [-90 ... 90] degrees and 
// yaw must be in the range of [0 ... 360] degrees.
// Pitch and yaw variables must be expressed in radians.
void Matrix4::FPSViewRH(Vec3D eye, double pitch, double yaw)
{
    // I assume the values are already converted to radians.
    double cosPitch = cos(pitch);
    double sinPitch = sin(pitch);
    double cosYaw = cos(yaw);
    double sinYaw = sin(yaw);

    Vec3D xaxis = { cosYaw, 0, -sinYaw };
    Vec3D yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
    Vec3D zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

    // Create a 4x4 view matrix from the right, up, forward and eye position vectors
//    mat4 viewMatrix = {
//        vec4(       xaxis.x,            yaxis.x,            zaxis.x,      0 ),
//        vec4(       xaxis.y,            yaxis.y,            zaxis.y,      0 ),
//        vec4(       xaxis.z,            yaxis.z,            zaxis.z,      0 ),
//        vec4( -dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye ), 1 )
//    };

/*
	//C  R
	x[0][0] = xaxis.x;
	x[0][1] = xaxis.y;
	x[0][2] = xaxis.z;
	x[0][3] = 0.0;

	x[1][0] = yaxis.x;
	x[1][1] = yaxis.y;
	x[1][2] = yaxis.z;
	x[1][3] = 0.0;

	x[2][0] = zaxis.x;
	x[2][1] = zaxis.y;
	x[2][2] = zaxis.z;
	x[2][3] = 0.0;

	x[3][0] = -xaxis.dot(eye);
	x[3][1] = -yaxis.dot(eye);
	x[3][2] = -zaxis.dot(eye);
*/
	//C  R
	x[0][0] = xaxis.x;
	x[1][0] = xaxis.y;
	x[2][0] = xaxis.z;
	x[3][0] = -xaxis.dot(eye);

	x[0][1] = yaxis.x;
	x[1][1] = yaxis.y;
	x[2][1] = yaxis.z;
	x[3][1] = -yaxis.dot(eye);

	x[0][2] = zaxis.x;
	x[1][2] = zaxis.y;
	x[2][2] = zaxis.z;
	x[3][2] = -zaxis.dot(eye);

	x[0][3] = 0.0;
	x[1][3] = 0.0;
	x[2][3] = 0.0;
	x[3][3] = 1.0;
}

void Matrix4::LookAtRH(Vec3D eye, Vec3D target, Vec3D up)
{
/*
	Vec3D zaxis = normal(eye - target);    // The "forward" vector.
	Vec3D xaxis = normal(cross(up, zaxis));// The "right" vector.
	Vec3D yaxis = cross(zaxis, xaxis);     // The "up" vector.

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
//	mat4 viewMatrix = {
//		vec4(      xaxis.x,            yaxis.x,            zaxis.x,       0 ),
//		vec4(      xaxis.y,            yaxis.y,            zaxis.y,       0 ),
//		vec4(      xaxis.z,            yaxis.z,            zaxis.z,       0 ),
//		vec4(-dot( xaxis, eye ), -dot( yaxis, eye ), -dot( zaxis, eye ),  1 )
//	};

	//C  R
	x[0][0] = xaxis.x;
	x[0][1] = xaxis.y;
	x[0][2] = xaxis.z;
	x[0][3] = -xaxis.dot(eye);

	x[1][0] = yaxis.x;
	x[1][1] = yaxis.y;
	x[1][2] = yaxis.z;
	x[1][3] = -yaxis.dot(eye);

	x[2][0] = zaxis.x;
	x[2][1] = zaxis.y;
	x[2][2] = zaxis.z;
	x[2][3] = -zaxis.dot(eye);

	x[3][0] = 0.0;
	x[3][1] = 0.0;
	x[3][2] = 0.0;
	x[3][3] = 1.0;
*/
}

///////////////////////////////////////////////////////////////////////////////

Vec3D Matrix4::crossProduct(Vec3D a, Vec3D b)
{
	Vec3D result;

	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	result.w = 0;

	return result;
}

Vec3D Matrix4::normalise(Vec3D a)
{
	Vec3D result;

	double len = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));

	result.x = a.x / len;
	result.y = a.y / len;
	result.z = a.z / len;

	return result;
}


// EOF
