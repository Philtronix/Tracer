#include "Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "cube.h"

// Add vertex to mesh, fix position to be on unit sphere, return index
int Cube::AddVertex(double x, double y, double z)
{
    data.push_back(Vec3D(x, y, z));              // Model
    tmp.push_back(Vec3D(0.0, 0.0, 0.0));         // Model
    normal.push_back(Vec3D(0.0, 0.0, 0.0));      // Model
    tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    numP++;

    return (numP - 1);
}

Cube::~Cube()
{

}

Cube::Cube(int length, int width, int height, Vec3D pos)
{
    double yt;  // Top
    double yb;  // Bottom
    double xl;  // Left
    double xr;  // Right
    double zf;  // Front
    double zb;  // Back

    yt = height / 2;
    yb = 0 - yt;

    xr = width / 2;
    xl = 0 - xr;

    zf = length / 2;
    zb = 0 - zf;
    
    AddVertex(xl, yt, zf);  // 1
    AddVertex(xr, yt, zf);  // 2
    AddVertex(xl, yb, zf);  // 3
    AddVertex(xr, yb, zf);  // 4
    AddVertex(xl, yt, zb);  // 5
    AddVertex(xr, yt, zb);  // 6
    AddVertex(xr, yb, zb);  // 7
    AddVertex(xl, yb, zb);  // 8

    surfaces.push_back(SURFACE{ 1, 2, 3 });
    surfaces.push_back(SURFACE{ 2, 3, 4 });
    surfaces.push_back(SURFACE{ 2, 4, 7 });
    surfaces.push_back(SURFACE{ 2, 6, 7 });
    surfaces.push_back(SURFACE{ 1, 2, 5 });
    surfaces.push_back(SURFACE{ 2, 5, 6 });
    surfaces.push_back(SURFACE{ 3, 4, 8 });
    surfaces.push_back(SURFACE{ 4, 7, 8 });
    surfaces.push_back(SURFACE{ 1, 3, 8 });
    surfaces.push_back(SURFACE{ 1, 5, 8 });
    surfaces.push_back(SURFACE{ 5, 6, 7 });
    surfaces.push_back(SURFACE{ 5, 7, 8 });

    // Other variables    
    numSurf  = 12;
    numP     = 8;
    zoom     = 1;
    position = pos;

	Mesh_normalise();

    printf("Rectangle : Vertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", numP, numNorm, numSurf);
}

// EOF
