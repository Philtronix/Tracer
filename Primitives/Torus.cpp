#include "../Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "Torus.h"

// Add vertex to mesh, fix position to be on unit sphere, return index
int Torus::AddVertex(double x, double y, double z)
{
    data.push_back(Vec3D(x, y, z));              // Model
    tmp.push_back(Vec3D(0.0, 0.0, 0.0));         // Model
    normal.push_back(Vec3D(0.0, 0.0, 0.0));      // Model
    tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    numP++;

    return (numP - 1);
}

Torus::~Torus()
{

}

// radial_resolution  = Number of points per segment
// tubular_resolution = number of "spokes"
// radius             = Outsode radius
// thickness          = Radius of Spun circle
Torus::Torus(int radial_resolution, int tubular_resolution, double radius, double thickness, Vec3D pos)
{
    // --- Generate vertices ---

    // Points per segment
    for (int i = 0; i < radial_resolution; i++) 
    {
        // Spokes
        for (int j = 0; j < tubular_resolution; j++)
        {
            double u = (double)j / tubular_resolution * M_PI * 2.0;
            double v = (double)i / radial_resolution * M_PI * 2.0;
            double x = (radius + thickness * std::cos(v)) * std::cos(u);
            double y = (radius + thickness * std::cos(v)) * std::sin(u);
            double z = thickness * std::sin(v);
            AddVertex(x, y, z);
        }
    }

    // ---- Add surfaces ----

    // Points per segment
    for (int i = 0; i < radial_resolution; i++)
    {
        // Spokes
        auto i_next = (i + 1) % radial_resolution;
        for (int j = 0; j < tubular_resolution; j++)
        {
            int j_next = (j + 1) % tubular_resolution;
            int i0 = i * tubular_resolution + j;
            int i1 = i * tubular_resolution + j_next;
            int i2 = i_next * tubular_resolution + j_next;
            int i3 = i_next * tubular_resolution + j;
            surfaces.push_back(SURFACE{ i0, i1, i3});
            surfaces.push_back(SURFACE{ i1, i3, i2});
        }
    }

    // Other variables    
    numSurf  = surfaces.size();
    numP     = (tubular_resolution * radial_resolution);
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

    printf("Torus :\r\nVertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", numP, numNorm, numSurf);
}

// EOF
