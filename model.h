#pragma once

#include <stdio.h>
#include "Vec3D.h"
#include "tracer.h"
#include "Matrix4.h"

#include <vector>

#define FLAT_RED   50.0
#define FLAT_GREEN 150.0
#define FLAT_BLUE  250.0

#define FLAT_BLACK ColourRef{240, 240, 240}
#define FLAT_WHITE ColourRef{15, 15, 15}

typedef struct SURFACETAG
{
	int	p1;
	int	p2;
	int	p3;

	int	t1;
	int	t2;
	int	t3;

	int	n1;
	int	n2;
	int	n3;

	ColourRef colour;
} SURFACE;

class Model
{
public:
	Model();
	~Model();
	
	bool LoadObjFile(const char *file);
	void Mesh_normalise();
    void SetColour(ColourRef colour);

	// TODO : Optimise - move to Vec3D ?
	std::vector<Vec3D> data;
	std::vector<Vec3D> tmp;
	std::vector<Vec3D> normal;		// Original data
	std::vector<Vec3D> tmpNorm;		// Rotated for display
	std::vector<Vec3D> showNorm;	// Rotated and moved for show normals

	std::vector<SURFACE> surfaces;

	int		numP;
	int		numSurf;
	int		numNorm;
	Vec3D	position;
	bool	show;
	char	name[100];
	double  zoom;
	double  rotateX;
	double  rotateY;
	double  rotateZ;

	// How to manipulate model to make it ove from object space to world space
	Matrix4 world;

private:
	int   ReadLine(char *dst, int *nBytes, FILE *fp);
	Vec3D Vector_CrossProduct(Vec3D &v1, Vec3D &v2);
};

// EOF
