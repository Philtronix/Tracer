#include "DrawGouraud.h"

#include "../tracer.h"
#include "../model.h"

extern Model      model[];
extern int        numModel;
extern int        zoom;
extern item       sortList[9000];
extern Vec3D      lightPos;
extern GdkPixbuf  *pixbuf;

// Local functions
static double CalcIntensity(double ya, double yb, double yc, double ia, double ib);
static void   CalcColour(double intensity, ColourRef *colour);
static double Interpolate(double min, double max, double gradient);
static void   ProcessScanLine(GdkPixbuf *pixbuf, int y, Point pa, Point pb, Point pc, Point pd, ColourRef colour);
static void   DrawTriangle(GdkPixbuf *pixbuf, Point p1, Point p2, Point p3, ColourRef colour);
static double ComputeNDotL(Vec3D vertex, Vec3D normal, Vec3D lightPosition);

//#define DEBUG(x) g_print(x)
#define DEBUG(x)

// Compute the cosine of the angle between the light vector and the normal vector
// Returns a value between 0 and 1
static double ComputeNDotL(Vec3D vertex, Vec3D normal, Vec3D lightPosition) 
{
    Vec3D lightDirection = lightPosition - vertex;

    normal.normalise();
    lightDirection.normalise();

	Vec3D tmp;
	double dt = tmp.dot(normal, lightDirection);

	if (dt > 0)
	{
		return dt;
	}
	else
	{
		return 0;
	}
}

void DrawGouraud(cairo_t *cr)
{
    int i;
	int	h = (VIEWSCRHEIGHT / 2);
	int	w = (VIEWSCRWIDTH / 2);
	int	p1;
	int	p2;
	int	p3;
	Vec3D v1;
	Vec3D v2;
	Vec3D v3;
	Vec3D n1;
	Vec3D n2;
	Vec3D n3;
    Point p2d1;
    Point p2d2;
    Point p2d3;
	ColourRef	colour;
	int		m;
	int		numSurf;

	numSurf = 0;
	for (int i = 0; i < numModel; i++)
	{
		numSurf += model[i].numSurf;
	}

    DEBUG("DrawGouraud()\r\n");
	for (i = 0; i < numSurf; i++)
	{
		// TODO : Replace sorted with Z buffer
        // Unsorted
//		p1 = model.surfaces[i].p1 - 1;
//		p2 = model.surfaces[i].p2 - 1;
//		p3 = model.surfaces[i].p3 - 1;
		m = sortList[i].model;

		// Sorted
		p1 = model[m].surfaces[sortList[i].surface].p1 - 1;
		p2 = model[m].surfaces[sortList[i].surface].p2 - 1;
		p3 = model[m].surfaces[sortList[i].surface].p3 - 1;

        // Filled triangle
        v1.x = (model[m].tmp[p1].x * zoom) + w;
        v1.y = (model[m].tmp[p1].y * zoom) + h;
        v1.z = (model[m].tmp[p1].z * zoom);

        v2.x = (model[m].tmp[p2].x * zoom) + w;
        v2.y = (model[m].tmp[p2].y * zoom) + h;
        v2.z = (model[m].tmp[p2].z * zoom);

        v3.x = (model[m].tmp[p3].x * zoom) + w;
        v3.y = (model[m].tmp[p3].y * zoom) + h;
        v3.z = (model[m].tmp[p3].z * zoom);

        // Normals
        n1 = model[m].tmpNorm[p1];
        n2 = model[m].tmpNorm[p2];
        n3 = model[m].tmpNorm[p3];

		// Vertex intensity
		p2d1.intensity = ComputeNDotL(model[m].tmp[p1], n1, lightPos);
		p2d2.intensity = ComputeNDotL(model[m].tmp[p2], n2, lightPos);
		p2d3.intensity = ComputeNDotL(model[m].tmp[p3], n3, lightPos);

        p2d1.vertex = v1;
        p2d2.vertex = v2;
        p2d3.vertex = v3;

        p2d1.normal = n1;
        p2d2.normal = n2;
        p2d3.normal = n3;

        colour.red   = FLAT_RED;
        colour.green = FLAT_GREEN;
        colour.blue  = FLAT_BLUE;

		DrawTriangle(pixbuf, p2d1, p2d2, p2d3, colour);

        // Now draw borders
        //cairo_move_to(cr, (int)v1.x, (int)v1.y);
        //cairo_line_to(cr, (int)v2.x, (int)v2.y);
        //cairo_line_to(cr, (int)v3.x, (int)v3.y);
        //cairo_line_to(cr, (int)v1.x, (int)v1.y);
    }
    DEBUG("DrawGouraud() - [done]\r\n");
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

// Interpolating the value between 2 vertices 
// min is the starting point, max the ending point
// and gradient the % between the 2 points
double Interpolate(double min, double max, double gradient)
{
	return min + (max - min) * Clamp(gradient);
}

// drawing line between 2 points from left to right
// papb -> pcpd
// pa, pb, pc, pd must then be sorted before
void ProcessScanLine(GdkPixbuf *pixbuf, int y, Point pa, Point pb, Point pc, Point pd, ColourRef colour)
{
	// Thanks to current Y, we can compute the gradient to compute others values like
	// the starting X (sx) and ending X (ex) to draw between
	// if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1
	double dbY = y;
	double gradient1 = pa.vertex.y != pb.vertex.y ? (dbY - pa.vertex.y) / (pb.vertex.y - pa.vertex.y) : 1;
	double gradient2 = pc.vertex.y != pd.vertex.y ? (dbY - pc.vertex.y) / (pd.vertex.y - pc.vertex.y) : 1;
			
	int sx = (int)Interpolate(pa.vertex.x, pb.vertex.x, gradient1);
	int ex = (int)Interpolate(pc.vertex.x, pd.vertex.x, gradient2);

    double snl = Interpolate(pa.intensity, pb.intensity, gradient1);
    double enl = Interpolate(pc.intensity, pd.intensity, gradient2);

	ColourRef cTmp;

	// Drawing a line from left (sx) to right (ex) 
	for (int x = sx; x < ex; x++)
	{
        double gradient = ((double)(x - sx)) / ((double)(ex - sx));

		// TODO : store Z buffer
//        double z = Interpolate(z1, z2, gradient);

        double ndotl = Interpolate(snl, enl, gradient);

        // Changing the color value using the cosine of the angle
        // between the light vector and the normal vector
		cTmp.red   = colour.red   * ndotl;
		cTmp.green = colour.green * ndotl;
		cTmp.blue  = colour.blue  * ndotl;
		PutPixel(pixbuf, x, y, cTmp.red, cTmp.green, cTmp.blue);
	}

	if (sx > ex)
	{
		g_print("error %d .. %d\r\n", sx, ex);
	}
}

// a = top point (lowest y)
// b = bottom point (highest y)
// c = point along a..b
// Returns intensity at c
static double CalcIntensity(double ya, double yb, double yc, double ia, double ib)
{
	double val;
	val =  (((yc - ya) / (yb - ya) * ib) +
		    ((yb - yc) / (yb - ya) * ia));

	if (val < 0)
	{
//		g_print("neg");
		val = 0.0;
	}

//	g_print("%.2f %.2f %.2f %.2f %.2f : %.2f\r\n", ya, yb, yc, ia, ib, val);
	return val;
}

static void CalcColour(double intensity, ColourRef *colour)
{
	int red;
	int green;
	int blue;

	// ambient
	intensity += 0.1;

	red   = FLAT_RED   * intensity;
	green = FLAT_GREEN * intensity;
	blue  = FLAT_BLUE  * intensity;

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

static void DrawTriangle(GdkPixbuf *pixbuf, Point p1, Point p2, Point p3, ColourRef colour)
{
	ColourRef	tmp = colour;
	double		intensity;
	double		dP1P2;
	double		dP1P3;

	// Sorting the points in order to always have this order on screen p1, p2 & p3
	// with p1 always up (thus having the Y the lowest possible to be near the top screen)
	// then p2 between p1 & p3
	if (p1.vertex.y > p2.vertex.y)
	{
		Point temp = p2;
		p2 = p1;
		p1 = temp;
	}

	if (p2.vertex.y > p3.vertex.y)
	{
		Point temp = p2;
		p2 = p3;
		p3 = temp;
	}

	if (p1.vertex.y > p2.vertex.y)
	{
		Point temp = p2;
		p2 = p1;
		p1 = temp;
	}

	// http://en.wikipedia.org/wiki/Slope
	// Computing inverse slopes
	if (p2.vertex.y - p1.vertex.y >= 0)
	{
		dP1P2 = (p2.vertex.x - p1.vertex.x) / (p2.vertex.y - p1.vertex.y);
	}
	else
	{
		dP1P2 = 0;
	}

	if (p3.vertex.y - p1.vertex.y > 0)
	{
		dP1P3 = (p3.vertex.x - p1.vertex.x) / (p3.vertex.y - p1.vertex.y);
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
		for (int y = (int)p1.vertex.y; y <= (int)p3.vertex.y; y++)
		{
			if (y < p2.vertex.y)
			{
				// 1..3, 1..2
				intensity = CalcIntensity(p1.vertex.y, p3.vertex.y, y, p1.intensity, p3.intensity);
				CalcColour(intensity, &tmp);
				ProcessScanLine(pixbuf, y, p1, p3, p1, p2, tmp);
			}
			else
			{
				// 1..3, 2..3
				intensity = CalcIntensity(p1.vertex.y, p3.vertex.y, y, p1.intensity, p3.intensity);
				CalcColour(intensity, &tmp);
				ProcessScanLine(pixbuf, y, p1, p3, p2, p3, tmp);
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
		for (int y = (int)p1.vertex.y; y <= (int)p3.vertex.y; y++)
		{
			if (y < p2.vertex.y)
			{
				// 1..2, 1..3
				intensity = CalcIntensity(p1.vertex.y, p2.vertex.y, y, p1.intensity, p2.intensity);
				CalcColour(intensity, &tmp);
				ProcessScanLine(pixbuf, y, p1, p2, p1, p3, tmp);
			}
			else
			{
				// 2..3, 1..3
				intensity = CalcIntensity(p2.vertex.y, p3.vertex.y, y, p2.intensity, p3.intensity);
				CalcColour(intensity, &tmp);
				ProcessScanLine(pixbuf, y, p2, p3, p1, p3, tmp);
			}
		}
	}
}

// EOF
