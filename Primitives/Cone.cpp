#include "../Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "Cone.h"

// Add vertex to mesh, fix position to be on unit sphere, return index
int Cone::AddVertex(double x, double y, double z)
{
    data.push_back(Vec3D(x, y, z));              // Model
    tmp.push_back(Vec3D(0.0, 0.0, 0.0));         // Model
    normal.push_back(Vec3D(0.0, 0.0, 0.0));      // Model
    tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    numP++;

    return (numP - 1);
}

Cone::~Cone()
{

}

Cone::Cone(int height, int radius, int resolution, Vec3D pos)
{
    // Add vertices subdividing a circle
    for (int i = 0; i < resolution; i++)
    {
        double ratio = ((double)i) / (double)resolution;
        double r = ratio * (M_PI * 2.0);
        double x = std::cos(r) * radius;
        double y = std::sin(r) * radius;
        AddVertex(x, y, 0.0);
    }

    // Top Point
    AddVertex(0.0, 0.0, height);
    int top = data.size()-1;

    // Centre of the base
    AddVertex(0.0, 0.0, 0.0);
    int base = data.size()-1;

    // generate triangular faces
    for (int i = 0; i < resolution; i++)
    {
        int ii = (i + 1) % resolution;
        surfaces.push_back(SURFACE{top, i, ii});
        surfaces.push_back(SURFACE{base, ii, i});
    }

    // Other variables
    numSurf  = surfaces.size();
    numP     = data.size();
    zoom     = 1;
    position = pos;

    // obj +1 correct
    for (int s = 0; s < numSurf; s++)
    {
        surfaces[s].p1++;
        surfaces[s].p2++;
        surfaces[s].p3++;
    }

	Mesh_normalise();

    printf("Cone :\r\nVertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", numP, numNorm, numSurf);
}

// EOF
