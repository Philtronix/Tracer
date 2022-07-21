#include "DrawGouraud.h"

#include "../tracer.h"
#include "../model.h"

extern Model      model;
extern item       sortList[9000];
extern Vec3D      lightPos;
extern GdkPixbuf  *pixbuf;
extern int        ScreenWidth;
extern int        ScreenHeight;

typedef struct SCANLINEDATAtag
{
    int currentY;
    double ndotla;
    double ndotlb;
    double ndotlc;
    double ndotld;
} SCANLINEDATA;

typedef struct
{
	Vec3D	vertex;
	Vec3D	normal;
	double	intensity;
} Point;

// Local functions
static void   FillFlatSideTriangle(GdkPixbuf *pixbuf, Point v1, Point v2, Point v3, ColourRef colour);
static void   SortVerticesAscendingByY(Point *v1, Point *v2, Point *v3);
static double ComputeNDotL(Vec3D vertex, Vec3D normal, Vec3D lightPosition);
static void   ProcessScanLine(GdkPixbuf *pixbuf, SCANLINEDATA data, Vec3D pa, Vec3D pb, Vec3D pc, Vec3D pd, ColourRef color);
static double Interpolate(double min, double max, double gradient);
static double Clamp(double value);
static void   DrawGouraudTriangle(GdkPixbuf *pixbuf, Vec3D v1, Vec3D v2, Vec3D v3, Vec3D n1, Vec3D n2, Vec3D n3, ColourRef colour);
int    Sign(int val);

//#define DEBUG(x) g_print(x)
#define DEBUG(x)

void DrawPhil(cairo_t *cr)
{
    int i;
	int	h = (ScreenHeight / 2);
	int	w = (ScreenWidth / 2);
	int	p1;
	int	p2;
	int	p3;
	Vec3D	v1;
	Vec3D	v2;
	Vec3D	v3;
	Vec3D	n1;
	Vec3D	n2;
	Vec3D	n3;
	ColourRef	colour;

    DEBUG("DrawGouraud() \r\n");

	SortSurfaces();
	for (i = 0; i < model.numSurf; i++)
	{
		// Unsorted
//			p1 = model.surfaces[i].p1 - 1;
//			p2 = model.surfaces[i].p2 - 1;
//			p3 = model.surfaces[i].p3 - 1;

		// Sorted
		p1 = model.surfaces[sortList[i].surface].p1 - 1;
		p2 = model.surfaces[sortList[i].surface].p2 - 1;
		p3 = model.surfaces[sortList[i].surface].p3 - 1;

		// Filled triangle
		v1.x = model.tmp[p1].x  + w;
		v1.y = model.tmp[p1].y + h;
		v1.z = model.tmp[p1].z;

		v2.x = model.tmp[p2].x + w;
		v2.y = model.tmp[p2].y + h;
		v2.z = model.tmp[p2].z;

		v3.x = model.tmp[p3].x + w;
		v3.y = model.tmp[p3].y + h;
		v3.z = model.tmp[p3].z;

		// Normals
		n1 = model.tmpNorm[p1];
		n2 = model.tmpNorm[p2];
		n3 = model.tmpNorm[p3];

		colour.red   = (uint8_t)FLAT_RED;
		colour.green = (uint8_t)FLAT_GREEN;
		colour.blue  = (uint8_t)FLAT_BLUE;

		// computing the cos of the angle between the light vector and the normal vector
		// it will return a value between 0 and 1 that will be used as the intensity of the color

		//g_print("lightPos = (%.2f, %.2f, %.2f)\r\n", lightPos.x, lightPos.y, lightPos.z);
		DrawGouraudTriangle(pixbuf, v1, v2, v3, n1, n2, n3, colour);
	}

    DEBUG("DrawGouraud() - [done]\r\n");
}

// Draw Gouraud filled Triangle
static void DrawGouraudTriangle(GdkPixbuf *pixbuf, 
								Vec3D v1, Vec3D v2, Vec3D v3, 
								Vec3D n1, Vec3D n2, Vec3D n3, 
								ColourRef colour)
{
	Point vt1;
	Point vt2;
	Point vt3;

	vt1.vertex = v1;
	vt2.vertex = v2;
	vt3.vertex = v3;
	vt1.normal = n1;
	vt2.normal = n2;
	vt3.normal = n3;

	// At first sort the three vertices by y-coordinate ascending, 
	// so vt1 is the topmost vertice
	SortVerticesAscendingByY(&vt1, &vt2, &vt3);
	
	// Here we know that v1.y <= v2.y <= v3.y
	// Bottom-flat triangle
	if (vt2.vertex.y == vt3.vertex.y)
	{
		FillFlatSideTriangle(pixbuf, vt1, vt2, vt3, colour);
	}
	// Top-flat triangle
	else if (vt1.vertex.y == vt2.vertex.y)
	{
		FillFlatSideTriangle(pixbuf, vt3, vt1, vt2, colour);
	} 
	else
	{
		// General case - split the triangle in a topflat and bottom-flat one
		Point vTmp;
		vTmp.vertex.x = (int)(vt1.vertex.x + ((double)(vt2.vertex.y - vt1.vertex.y) / (double)(vt3.vertex.y - vt1.vertex.y)) * (vt3.vertex.x - vt1.vertex.x));
		vTmp.vertex.y = vt2.vertex.y;
		vTmp.vertex.z = vt2.vertex.z;
		vTmp.normal = vt2.normal;
		FillFlatSideTriangle(pixbuf, vt1, vt2, vTmp, colour);
		FillFlatSideTriangle(pixbuf, vt3, vt2, vTmp, colour);
	}
}

// This method rasterizes a triangle using only integer variables by using a
// modified bresenham algorithm.
// It's important that v2 and v3 lie on the same horizontal line, that is
// v2.y must be equal to v3.y.
static void FillFlatSideTriangle(GdkPixbuf *pixbuf, Point v1, Point v2, Point v3, ColourRef colour)
{
	Point vTmp1 = v1;
	Point vTmp2 = v1;
	
	bool changed1 = false;
	bool changed2 = false;
	
	int dx1 = abs(v2.vertex.x - v1.vertex.x);
	int dy1 = abs(v2.vertex.y - v1.vertex.y);
	
	int dx2 = abs(v3.vertex.x - v1.vertex.x);
	int dy2 = abs(v3.vertex.y - v1.vertex.y);
	
	int signx1 = Sign(v2.vertex.x - v1.vertex.x);
	int signx2 = Sign(v3.vertex.x - v1.vertex.x);
	
	int signy1 = Sign(v2.vertex.y - v1.vertex.y);
	int signy2 = Sign(v3.vertex.y - v1.vertex.y);

	SCANLINEDATA data;

	if (dy1 > dx1)
	{ 
		// Swap values
		int tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = true;
	}
	
	if (dy2 > dx2)
	{
		// Swap values
		int tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = true;
	}
	
	int e1 = 2 * dy1 - dx1;
	int e2 = 2 * dy2 - dx2;
	
	if (dx1 != 0)
	{
		double nl1 = ComputeNDotL(v1.vertex, v1.normal, lightPos);
		double nl2 = ComputeNDotL(v2.vertex, v2.normal, lightPos);
		double nl3 = ComputeNDotL(v3.vertex, v3.normal, lightPos);
/*
		g_print("%.2f %.2f %.2f - ", nl1, nl2, nl3);
		g_print("%.2f %.2f %.2f : ", v1.vertex.x, v1.vertex.y, v1.vertex.z);
		g_print("%.2f %.2f %.2f : ", v2.vertex.x, v2.vertex.y, v2.vertex.z);
		g_print("%.2f %.2f %.2f\r\n", v3.vertex.x, v3.vertex.y, v3.vertex.z);
*/

		for (int i = 0; i <= dx1; i++)
		{
			// Draw line
			if (v2.vertex.x > v3.vertex.x)
			{
				data.currentY = vTmp1.vertex.y;
				data.ndotla   = nl1;	// Normals
				data.ndotlb   = nl3;
				data.ndotlc   = nl1;
				data.ndotld   = nl2;
				ProcessScanLine(pixbuf, data, v1.vertex, v3.vertex, v1.vertex, v2.vertex, colour);
			}
			else
			{
				data.currentY = vTmp1.vertex.y;
				data.ndotla   = nl1;	// Normals
				data.ndotlb   = nl2;
				data.ndotlc   = nl1;
				data.ndotld   = nl3;
				ProcessScanLine(pixbuf, data, v1.vertex, v2.vertex, v1.vertex, v3.vertex, colour);
			}
			
			while (e1 >= 0)
			{
				if (changed1)
				{
					vTmp1.vertex.x += signx1;
				}
				else
				{
					vTmp1.vertex.y += signy1;
				}
				e1 = e1 - 2 * dx1;
			}
			
			if (changed1)
			{
				vTmp1.vertex.y += signy1;
			}
			else
			{
				vTmp1.vertex.x += signx1;
			}
			
			e1 = e1 + 2 * dy1;
			
			// Here we rendered the next point on line 1 so follow now line 2
			// until we are on the same y-value as line 1.
			while (vTmp2.vertex.y != vTmp1.vertex.y)
			{
				while (e2 >= 0)
				{
					if (changed2)
					{
						vTmp2.vertex.x += signx2;
					}
					else
					{
						vTmp2.vertex.y += signy2;
					}
					e2 = e2 - 2 * dx2;
				}

				if (changed2)
				{
					vTmp2.vertex.y += signy2;
				}
				else
				{
					vTmp2.vertex.x += signx2;
				}

				e2 = e2 + 2 * dy2;
			}
		}
	}
}

// Sort the three vertices ascending by y-coordinate.
// After calling following applies: vt1.y <= vt2.y <= vt3.y
void SortVerticesAscendingByY(Point *v1, Point *v2, Point *v3)
{
	Point	vTmp;

	if (v1->vertex.y > v2->vertex.y)
	{
		vTmp = *v1;
		*v1 = *v2;
		*v2 = vTmp;
	}

	// Here v1.y <= v2.y
	if (v1->vertex.y > v3->vertex.y)
	{
		vTmp = *v1;
		*v1 = *v3;
		*v3 = vTmp;
	}

	// Here v1.y <= v2.y and v1.y <= v3.y so test v2 vs. v3
	if (v2->vertex.y > v3->vertex.y)
	{
		vTmp = *v2;
		*v2 = *v3;
		*v3 = vTmp;
	}
}

// Compute the cosine of the angle between the light vector and the normal vector
// Returns a value between 0 and 1
static double ComputeNDotL(Vec3D vertex, Vec3D normal, Vec3D lightPosition) 
{
	double retVal;

    Vec3D lightDirection = lightPosition - vertex;

    normal.normalise();
    lightDirection.normalise();

    double val;
	val = normal.dot(lightDirection);

	if (val < 0)
	{
		retVal = 0;
	}
	else
	{
		retVal = val;
	}

	return retVal;
}

static void ProcessScanLine(GdkPixbuf *pixbuf, SCANLINEDATA data, Vec3D pa, Vec3D pb, Vec3D pc, Vec3D pd, ColourRef color)
{
    // Thanks to current Y, we can compute the gradient to compute others values like
    // the starting X (sx) and ending X (ex) to draw between
    // if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1
    double gradient1 = pa.y != pb.y ? (data.currentY - pa.y) / (pb.y - pa.y) : 1;
    double gradient2 = pc.y != pd.y ? (data.currentY - pc.y) / (pd.y - pc.y) : 1;

    int sx = (int)Interpolate(pa.x, pb.x, gradient1);
    int ex = (int)Interpolate(pc.x, pd.x, gradient2);

    // starting Z & ending Z
    double z1 = Interpolate(pa.z, pb.z, gradient1);
    double z2 = Interpolate(pc.z, pd.z, gradient2);

    // drawing a line from left (sx) to right (ex) 
	guchar red;
	guchar green;
	guchar blue;
    for (int x = sx; x < ex; x++)
    {
        double gradient = (x - sx) / (double)(ex - sx);

        double z = Interpolate(z1, z2, gradient);
        double ndotl = data.ndotla;
        // changing the color value using the cosine of the angle
        // between the light vector and the normal vector
//        DrawPoint(new Vector3(x, data.currentY, z), color * ndotl);

		red   = color.red * ndotl;
		green = color.green * ndotl;
		blue  = color.blue * ndotl;

//		g_print("ndotl = %f\r\n", ndotl);
		PutPixel(pixbuf, x, data.currentY, red, green, blue);
    }
}

// Interpolating the value between 2 vertices 
// min is the starting point, max the ending point
// and gradient the % between the 2 points
static double Interpolate(double min, double max, double gradient)
{
    return min + (max - min) * Clamp(gradient);
}

// Clamping values to keep them between 0 and 1
static double Clamp(double value)
{
	double retVal;

	if (value > 1.0)
	{
		retVal = 1.0;
	}
	else if (value < 0.0)
	{
		retVal = 0.0;
	}
	else 
	{
		retVal = value;
	}

	return retVal;
}

// Java sdn function
int Sign(int val)
{
	int vRet = 0;

	if (val > 0)
	{
		vRet = 1;
	}
	else if (val < 0)
	{
		vRet = -1;
	}
	else
	{
		vRet = 0;
	}

	return vRet;
}

