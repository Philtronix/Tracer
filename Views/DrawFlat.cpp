#include "DrawFlat.h"

#include "../tracer.h"
#include "../model.h"

extern Model      model;
extern int        zoom;
extern item       sortList[9000];
extern Vec3D      lightPos;
extern GdkPixbuf  *pixbuf;

/*
typedef struct
{
	int x;
	int y;
} Point2D;
*/

// Local functions
//static void FillFlatSideTriangle(GdkPixbuf *pixbuf, Point2D v1, Point2D v2, Point2D v3, ColourRef colour);
//static void DrawFlatFilledTriangle(GdkPixbuf *pixbuf, Point2D v1, Point2D v2, Point2D v3, ColourRef colour);
//static void DrawLine(GdkPixbuf *pixbuf, Point2D p1, Point2D p2, ColourRef colour);
//static void SortVerticesAscendingByY(Point2D *v1, Point2D *v2, Point2D *v3);

// TEST
static double Interpolate(double min, double max, double gradient);
static void   ProcessScanLine(GdkPixbuf *pixbuf, int y, Vec3D pa, Vec3D pb, Vec3D pc, Vec3D pd, ColourRef colour);
static void   DrawTriangle(GdkPixbuf *pixbuf, Vec3D p1, Vec3D p2, Vec3D p3, ColourRef colour);
static double Clamp(double value);
static double bob(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D n1, Vec3D n2, Vec3D n3);
static void CalcColour(double angle, ColourRef *colour);
// TEST

//#define DEBUG(x) g_print(x)
#define DEBUG(x)

void DrawFlat(cairo_t *cr)
{
    int i;
	int	h = (VIEWSCRHEIGHT / 2);
	int	w = (VIEWSCRWIDTH / 2);
	int	p1;
	int	p2;
	int	p3;
	Vec3D	v1;
	Vec3D	v2;
	Vec3D	v3;
	Vec3D	n1;
	Vec3D	n2;
	Vec3D	n3;
//    Point2D p2d1;
//    Point2D p2d2;
//    Point2D p2d3;
	ColourRef	colour;

    DEBUG("DrawFlat() \r\n");
    for (i = 0; i < model.numSurf; i++)
    {
        // Unsorted
//		p1 = model.surfaces[i].p1 - 1;
//		p2 = model.surfaces[i].p2 - 1;
//		p3 = model.surfaces[i].p3 - 1;

        // Sorted
        p1 = model.surfaces[sortList[i].surface].p1 - 1;
        p2 = model.surfaces[sortList[i].surface].p2 - 1;
        p3 = model.surfaces[sortList[i].surface].p3 - 1;

        // Filled triangle
        v1.x = (model.tmp[p1].x * zoom) + w;
        v1.y = (model.tmp[p1].y * zoom) + h;
        v1.z = (model.tmp[p1].z * zoom);

        v2.x = (model.tmp[p2].x * zoom) + w;
        v2.y = (model.tmp[p2].y * zoom) + h;
        v2.z = (model.tmp[p2].z * zoom);

        v3.x = (model.tmp[p3].x * zoom) + w;
        v3.y = (model.tmp[p3].y * zoom) + h;
        v3.z = (model.tmp[p3].z * zoom);

        // Normals
        n1 = model.tmpNorm[p1];
        n2 = model.tmpNorm[p2];
        n3 = model.tmpNorm[p3];

		//g_print("view filled\r\n");
		// Work out surface colour
/*
		Vec3D surfaceNormal = GetSurfaceNormal(v1, v2, v3);
        
        // Light position 
        //g_print("lightPos = (%.2f, %.2f, %.2f)\r\n", lightPos.x, lightPos.y, lightPos.z);
        Vec3D tmp = lightPos;
        tmp.normalise();
        double angle = DotProduct(surfaceNormal, tmp);

        // Bounds from 0 to Pi/2
        if (angle > (PI / 2.0))
        {
            angle = (PI / 2.0);
        }
        else if (angle < 0)
        {
            angle = 0;
        }

        // Normalise angle
        angle /= (PI / 2.0);
*/
		double angle = bob(v1, v2, v3, n1, n2, n3);

		CalcColour(angle, &colour);

        //DrawFlatFilledTriangle(pixbuf, p2d1, p2d2, p2d3, colour);
        DrawTriangle(pixbuf, v1, v2, v3, colour); // TEST

        // Now draw borders
        //cairo_move_to(cr, (int)v1.x, (int)v1.y);
        //cairo_line_to(cr, (int)v2.x, (int)v2.y);
        //cairo_line_to(cr, (int)v3.x, (int)v3.y);
        //cairo_line_to(cr, (int)v1.x, (int)v1.y);
    }
    DEBUG("DrawFlat() - [done]\r\n");
}

static void CalcColour(double angle, ColourRef *colour)
{
	int red;
	int green;
	int blue;

	// ambient
	angle += 0.1;

	red   = FLAT_RED   * angle;
	green = FLAT_GREEN * angle;
	blue  = FLAT_BLUE  * angle;

	if (red > 255)
	{
		red = 255;
	}

	if (green > 255)
	{
		green = 255;
	}

	if (blue > 255)
	{
		blue = 255;
	}

	colour->red   = (uint8_t)red;
	colour->green = (uint8_t)green;
	colour->blue  = (uint8_t)blue;
}

static double ComputeNDotL(Vec3D vertex, Vec3D normal, Vec3D lightPosition) 
{
	double retVal;

    Vec3D lightDirection = lightPosition - vertex;

    normal.normalise();
    lightDirection.normalise();

    double val;
	Vec3D tmp;
    val = tmp.dot(normal, lightDirection);

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

static double bob(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D n1, Vec3D n2, Vec3D n3)
{
    // Normal face's vector is the average normal between each vertex's normal
    // computing also the center point of the face
	Vec3D vnFace;
	vnFace.x = (n1.x + n2.x + n3.x) / 3;
	vnFace.y = (n1.y + n2.y + n3.y) / 3;
	vnFace.z = (n1.z + n2.z + n3.z) / 3;
 	
	Vec3D centerPoint;
	centerPoint.x = (v1.x + v2.x + v3.x) / 3;
	centerPoint.y = (v1.y + v2.y + v3.y) / 3;
	centerPoint.z = (v1.z + v2.z + v3.z) / 3;

    // computing the cos of the angle between the light vector and the normal vector
    // it will return a value between 0 and 1 that will be used as the intensity of the color
    return ComputeNDotL(centerPoint, vnFace, lightPos);
 }

/*
// Draw flat filled Triangle
static void DrawFlatFilledTriangle(GdkPixbuf *pixbuf, Point2D v1, Point2D v2, Point2D v3, ColourRef colour)
{
	Point2D vt1 = v1;
	Point2D vt2 = v2;
	Point2D vt3 = v3;

    DEBUG("DrawFlatFilledTriangle()\r\n");

	// At first sort the three vertices by y-coordinate ascending, 
	// so p1 is the topmost vertice
	SortVerticesAscendingByY(&vt1, &vt2, &vt3);
	
	// Here we know that v1.y <= v2.y <= v3.y
	// Bottom-flat triangle
	if (vt2.y == vt3.y)
	{
		FillFlatSideTriangle(pixbuf, vt1, vt2, vt3, colour);
	}
	// Top-flat triangle
	else if (vt1.y == vt2.y)
	{
		FillFlatSideTriangle(pixbuf, vt3, vt1, vt2, colour);
	} 
	else
	{
		// General case - split the triangle in a topflat and bottom-flat one
		Point2D vTmp;
		vTmp.x = (int)(vt1.x + ((float)(vt2.y - vt1.y) / (float)(vt3.y - vt1.y)) * (vt3.x - vt1.x));
		vTmp.y = vt2.y;
		FillFlatSideTriangle(pixbuf, vt1, vt2, vTmp, colour);
		FillFlatSideTriangle(pixbuf, vt3, vt2, vTmp, colour);
	}

    DEBUG("DrawFlatFilledTriangle() - [done]\r\n");
}
*/

/*
// Sort the three vertices ascending by y-coordinate.
// After calling following applies: vt1.y <= vt2.y <= vt3.y
static void SortVerticesAscendingByY(Point2D *v1, Point2D *v2, Point2D *v3)
{
	Point2D	vTmp;

	if (v1->y > v2->y)
	{
		vTmp = *v1;
		*v1 = *v2;
		*v2 = vTmp;
	}

	// Here v1.y <= v2.y
	if (v1->y > v3->y)
	{
		vTmp = *v1;
		*v1 = *v3;
		*v3 = vTmp;
	}

	// Here v1.y <= v2.y and v1.y <= v3.y so test v2 vs. v3
	if (v2->y > v3->y)
	{
		vTmp = *v2;
		*v2 = *v3;
		*v3 = vTmp;
	}
}
*/

/*
// This method rasterizes a triangle using only integer variables by using a
// modified bresenham algorithm.
// It's important that v2 and v3 lie on the same horizontal line, that is
// v2.y must be equal to v3.y.
static void FillFlatSideTriangle(GdkPixbuf *pixbuf, Point2D v1, Point2D v2, Point2D v3, ColourRef colour)
{
    DEBUG("FillFlatSideTriangle() ");

	Point2D vTmp1 = v1;
	Point2D vTmp2 = v1;
	
	bool changed1 = false;
	bool changed2 = false;
	
	int dx1 = abs(v2.x - v1.x);
	int dy1 = abs(v2.y - v1.y);
	
	int dx2 = abs(v3.x - v1.x);
	int dy2 = abs(v3.y - v1.y);
	
	int signx1 = Sign(v2.x - v1.x);
	int signx2 = Sign(v3.x - v1.x);
	
	int signy1 = Sign(v2.y - v1.y);
	int signy2 = Sign(v3.y - v1.y);
	
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

    // Prevent exception
    if (0 == dx1)
    {
        g_print("Boom\r\n");
        dx1 = 1;
    }

	for (int i = 0; i <= dx1; i++)
	{
		// Draw line
		DrawLine(pixbuf, vTmp1, vTmp2, colour);

		while (e1 >= 0)
		{
			if (changed1)
			{
				vTmp1.x += signx1;
			}
			else
			{
				vTmp1.y += signy1;
			}
			e1 = e1 - 2 * dx1;
		}
		
		if (changed1)
		{
			vTmp1.y += signy1;
		}
		else
		{
			vTmp1.x += signx1;
		}
		
		e1 = e1 + 2 * dy1;
		
//        DEBUG("2");
		// Here we rendered the next point on line 1 so follow now line 2
		// until we are on the same y-value as line 1.
		while (vTmp2.y != vTmp1.y)
		{
			while (e2 >= 0)
			{
				if (changed2)
				{
					vTmp2.x += signx2;
				}
				else
				{
					vTmp2.y += signy2;
				}
				e2 = e2 - 2 * dx2;
			}

			if (changed2)
			{
				vTmp2.y += signy2;
			}
			else
			{
				vTmp2.x += signx2;
			}

			e2 = e2 + 2 * dy2;
		}
	}

    DEBUG("- [done]\r\n");
}
*/

/*
static void DrawLine(GdkPixbuf *pixbuf, Point2D p1, Point2D p2, ColourRef colour)
{
	int x1;
	int x2;

	// Ensure left - right drawing
	if (p1.x < p2.x)
	{
		x1 = p1.x;
		x2 = p2.x;
	}
	else
	{
		x1 = p2.x;
		x2 = p1.x;
	}

	for (int x = x1; x <= x2; x++)
	{
		PutPixel(pixbuf, x, p1.y, colour.red, colour.green, colour.blue);
	}
}
*/

///////////////////////////////////////////////////////////////////////////////
// New test routines

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

// Interpolating the value between 2 vertices 
// min is the starting point, max the ending point
// and gradient the % between the 2 points
static double Interpolate(double min, double max, double gradient)
{
	return min + (max - min) * Clamp(gradient);
}

// drawing line between 2 points from left to right
// papb -> pcpd
// pa, pb, pc, pd must then be sorted before
static void ProcessScanLine(GdkPixbuf *pixbuf, int y, Vec3D pa, Vec3D pb, Vec3D pc, Vec3D pd, ColourRef colour)
{
	// Thanks to current Y, we can compute the gradient to compute others values like
	// the starting X (sx) and ending X (ex) to draw between
	// if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1
	double gradient1 = pa.y != pb.y ? (y - pa.y) / (pb.y - pa.y) : 1;
	double gradient2 = pc.y != pd.y ? (y - pc.y) / (pd.y - pc.y) : 1;
			
	int sx = (int)Interpolate(pa.x, pb.x, gradient1);
	int ex = (int)Interpolate(pc.x, pd.x, gradient2);

	if (sx > ex)
	{
		g_print("error %d .. %d - RED\r\n", sx, ex);
		int tmp = ex;
		ex = sx;
		sx = tmp;
		colour.red   = 255;
		colour.green = 0;
		colour.blue  = 0;
	}

	// drawing a line from left (sx) to right (ex) 
	for (int x = sx; x < ex; x++)
	{
		PutPixel(pixbuf, x, y, colour.red, colour.green, colour.blue);
	}
}

static void DrawTriangle(GdkPixbuf *pixbuf, Vec3D p1, Vec3D p2, Vec3D p3, ColourRef colour)
{
	// Sorting the points in order to always have this order on screen p1, p2 & p3
	// with p1 always up (thus having the Y the lowest possible to be near the top screen)
	// then p2 between p1 & p3
	if (p1.y > p2.y)
	{
		Vec3D temp = p2;
		p2 = p1;
		p1 = temp;
	}

	if (p2.y > p3.y)
	{
		Vec3D temp = p2;
		p2 = p3;
		p3 = temp;
	}

	if (p1.y > p2.y)
	{
		Vec3D temp = p2;
		p2 = p1;
		p1 = temp;
	}

	// inverse slopes
	double dP1P2;
	double dP1P3;

	// http://en.wikipedia.org/wiki/Slope
	// Computing inverse slopes
	if (p2.y - p1.y >= 0)	// Fixes some left / right
	{
		dP1P2 = (p2.x - p1.x) / (p2.y - p1.y);
	}
	else
	{
		dP1P2 = 0;
	}

	if (p3.y - p1.y > 0)
	{
		dP1P3 = (p3.x - p1.x) / (p3.y - p1.y);
	}
	else
	{
		dP1P3 = 0;
	}

	// First case where triangles are like that:
	// P1         Lowest Y
	// -
	// -- 
	// - -
	// -  -
	// -   - P2
	// -  -
	// - -
	// -
	// P3         Highest Y
	if (dP1P2 > dP1P3)
	{
		for (int y = (int)p1.y; y <= (int)p3.y; y++)
		{
			if (y < p2.y)
			{
				// 1..3, 1..2
				ProcessScanLine(pixbuf, y, p1, p3, p1, p2, colour); // err
			}
			else
			{
				// 1..3, 2..3
				ProcessScanLine(pixbuf, y, p1, p3, p2, p3, colour);
			}
		}
	}
	// First case where triangles are like that:
	//       P1  Lowest Y
	//        -
	//       -- 
	//      - -
	//     -  -
	// P2 -   - 
	//     -  -
	//      - -
	//        -
	//       P3  Highest Y
	else
	{
		for (int y = (int)p1.y; y <= (int)p3.y; y++)
		{
			if (y < p2.y)
			{
				// 1..2, 1..3
				ProcessScanLine(pixbuf, y, p1, p2, p1, p3, colour); // err
			}
			else
			{
				// 2..3, 1..3
				ProcessScanLine(pixbuf, y, p2, p3, p1, p3, colour);
			}
		}
	}
}

// EOF
