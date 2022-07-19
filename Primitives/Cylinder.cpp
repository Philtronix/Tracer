#include "../Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "Cylinder.h"

// Add vertex to mesh, fix position to be on unit sphere, return index
int Cylinder::AddVertex(double x, double y, double z)
{
    data.push_back(Vec3D(x, y, z));              // Model
    tmp.push_back(Vec3D(0.0, 0.0, 0.0));         // Model
    normal.push_back(Vec3D(0.0, 0.0, 0.0));      // Model
    tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    numP++;

    return (numP - 1);
}

Cylinder::~Cylinder()
{

}

Cylinder::Cylinder(int height, int radius, int resolution, Vec3D pos)
{
    // Generate points
    for (int i = 0; i < resolution; i++)
    {
        double ratio = ((double)i) / (double)resolution;
        double r = ratio * (M_PI * 2.0);
        double x = std::cos(r) * radius;
        double y = std::sin(r) * radius;

        // Bottom
        AddVertex(x, y, 0.0);

        // Top
        AddVertex(x, y, height);
    }

    // Centre points
    AddVertex(0.0, 0.0, 0.0);
    AddVertex(0.0, 0.0, height);
    int top = data.size() - 2;
    int bottom = data.size() - 1;

    // add faces around the cylinder
    for (int i = 0; i < resolution; i++)
    {
        int ii = i * 2;
        int jj = (ii + 2) % (resolution * 2);
        int kk = (ii + 3) % (resolution * 2);
        int ll = ii + 1;
        surfaces.push_back(SURFACE{ii, jj, kk});
        surfaces.push_back(SURFACE{ii, kk, ll});

        surfaces.push_back(SURFACE{jj, ii, top});
        surfaces.push_back(SURFACE{ll, kk, bottom});
    }

    // Other variables    
    numSurf  = surfaces.size();
    numP     = data.size();
    zoom     = 1;
    numNorm  = 0;
    position = pos;

    // obj +1 correct
    for (int s = 0; s < numSurf; s++)
    {
        surfaces[s].p1++;
        surfaces[s].p2++;
        surfaces[s].p3++;
    }

	Mesh_normalise();

    printf("Cylinder :\r\nVertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", numP, numNorm, numSurf);
}

// EOF
