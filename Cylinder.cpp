#include "Vec3D.h"
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
/*
    double yt;  // Top
    double yb;  // Bottom
    double xl;  // Left
    double xr;  // Right
    double zf;  // Front
    double zb;  // Back

    yb = height / 2;
    yt = 0 - yb;

    xl = width / 2;
    xr = 0 - xl;

    zf = length / 2;
    zb = 0 - zf;
    
    // Points
    AddVertex(xl, yt, zf);  // 1
    AddVertex(xr, yt, zf);  // 2
    AddVertex(xl, yb, zf);  // 3
    AddVertex(xr, yb, zf);  // 4
    AddVertex(xl, yt, zb);  // 5
    AddVertex(xr, yt, zb);  // 6
    AddVertex(xr, yb, zb);  // 7
    AddVertex(xl, yb, zb);  // 8

    // Normals
    normal[0] = Vec3D( 1.0, -1.0,  1.0);    // Top. left
    normal[1] = Vec3D(-1.0, -1.0,  1.0);    // Top
    normal[2] = Vec3D( 1.0,  1.0,  1.0);
    normal[3] = Vec3D(-1.0,  1.0,  1.0);    // Left
    normal[4] = Vec3D( 1.0, -1.0, -1.0);    // Top, left
    normal[5] = Vec3D(-1.0, -1.0, -1.0);    // Top
    normal[6] = Vec3D(-1.0,  1.0, -1.0);    // Left
    normal[7] = Vec3D( 1.0,  1.0, -1.0);

    // Surfaces
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
*/

//    SurfaceMesh cylinder(size_t resolution, Scalar radius, Scalar height)

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

        g_print("%d : %.2f %.2f\r\n", i, x, y);
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
