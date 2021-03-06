#pragma once

#include "Vec3D.h"
#include <sys/types.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

//#define SWAP(a,b)  { Vec3D *tmp; tmp = a; a = b; b = tmp; }
#define SUB_PIX(a) (ceil(a)-a)
#define PI         3.147

#define VIEW_POINTS		1
#define VIEW_WIRE		2
#define VIEW_FLAT		3
#define VIEW_GOURAUD	4
#define VIEW_PHONG		5
#define VIEW_RAYTRACE	6

#define VIEWSCRWIDTH	640
#define VIEWSCRHEIGHT	480

struct ColourRef
{
	uint8_t	red;
	uint8_t	green;
	uint8_t	blue;
};

typedef struct
{
	double	avz;
	int		model;
	int		surface;
} item;

void   PutPixel(GdkPixbuf *pixbuf, int x, int y, guchar red, guchar green, guchar blue);
void   SortSurfaces();
Vec3D  GetSurfaceNormal(Vec3D a, Vec3D b, Vec3D c);
double DotProduct(Vec3D a, Vec3D b);

