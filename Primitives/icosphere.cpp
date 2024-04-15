#include "../Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "icosphere.h"

// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

// Add vertex to mesh, fix position to be on unit sphere, return index
int Icosphere::AddVertex(double x, double y, double z)
{
    double length = sqrt((x * x) + (y * y) + (z * z));
    double vx = x / length;
    double vy = y / length;
    double vz = z / length;

    data.push_back(Vec3D(vx, vy, vz));           // Model
    tmp.push_back(Vec3D(0.0, 0.0, 0.0));         // Model
    normal.push_back(Vec3D(0.0, 0.0, 0.0));      // Model
    tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    numP++;

    return (numP - 1);
}

// Return index of point in the middle of p1 and p2
int Icosphere::GetMiddlePoint(int p1, int p2)
{
    // First check if we have it already
    bool firstIsSmaller = p1 < p2;
    int  smallerIndex = firstIsSmaller ? p1 : p2;
    int  greaterIndex = firstIsSmaller ? p2 : p1;
    int  key = (smallerIndex << 16) + greaterIndex;

    auto item = middlePointIndexCache.find(key);

    if (item != middlePointIndexCache.end())
    {
        // Found
        return item->second;
    }

    // Not in cache, calculate it
    Vec3D point1 = data[p1];
    Vec3D point2 = data[p2];
    Vec3D middle = Vec3D((point1.x + point2.x) / 2.0, 
                         (point1.y + point2.y) / 2.0, 
                         (point1.z + point2.z) / 2.0);

    // add vertex makes sure point is on unit sphere
    int i = AddVertex(middle.x, middle.y, middle.z); 

    // Store it, return index
    middlePointIndexCache.insert({key, i});
    return i;
}

Icosphere::~Icosphere()
{

}

Icosphere::Icosphere(int radius, int recursionLevel, Vec3D pos)
{
    // Create 12 vertices of a icosahedron
    double t = (1.0 + sqrt(5.0)) / 2.0;
    int a;
    int b;
    int c;

    AddVertex(-1,  t,  0);  //  0
    AddVertex( 1,  t,  0);  //  1
    AddVertex(-1, -t,  0);  //  2
    AddVertex( 1, -t,  0);  //  3

    AddVertex( 0, -1,  t);  //  4
    AddVertex( 0,  1,  t);  //  5
    AddVertex( 0, -1, -t);  //  6
    AddVertex( 0,  1, -t);  //  7

    AddVertex( t,  0, -1);  //  8
    AddVertex( t,  0,  1);  //  9
    AddVertex(-t,  0, -1);  // 10
    AddVertex(-t,  0,  1);  // 11

    // Create 20 triangles of the icosahedron

    // 5 faces around point 0
    surfaces.push_back(SURFACE{0, 11,  5});
    surfaces.push_back(SURFACE{0,  5,  1});
    surfaces.push_back(SURFACE{0,  1,  7});
    surfaces.push_back(SURFACE{0,  7, 10});
    surfaces.push_back(SURFACE{0, 10, 11});

    // 5 adjacent faces 
    surfaces.push_back(SURFACE{ 1,  5, 9});
    surfaces.push_back(SURFACE{ 5, 11, 4});
    surfaces.push_back(SURFACE{11, 10, 2});
    surfaces.push_back(SURFACE{10,  7, 6});
    surfaces.push_back(SURFACE{ 7,  1, 8});

    // 5 faces around point 3
    surfaces.push_back(SURFACE{3, 9, 4});
    surfaces.push_back(SURFACE{3, 4, 2});
    surfaces.push_back(SURFACE{3, 2, 6});
    surfaces.push_back(SURFACE{3, 6, 8});
    surfaces.push_back(SURFACE{3, 8, 9});

    // 5 adjacent faces 
    surfaces.push_back(SURFACE{4, 9,  5});
    surfaces.push_back(SURFACE{2, 4, 11});
    surfaces.push_back(SURFACE{6, 2, 10});
    surfaces.push_back(SURFACE{8, 6,  7});
    surfaces.push_back(SURFACE{9, 8,  1});

    std::vector<SURFACE> faces2;

    // Refine triangles
    for (int i = 0; i < recursionLevel; i++)
    {
        faces2.clear();
        for (int s = 0; s < (int) surfaces.size(); s++)
        {
            // Replace triangle by 4 triangles
            // Creates or finds the point and returns its index
            a = GetMiddlePoint(surfaces[s].p1, surfaces[s].p2);
            b = GetMiddlePoint(surfaces[s].p2, surfaces[s].p3);
            c = GetMiddlePoint(surfaces[s].p3, surfaces[s].p1);

            faces2.push_back(SURFACE{surfaces[s].p1, a, c});
            faces2.push_back(SURFACE{surfaces[s].p2, b, a});
            faces2.push_back(SURFACE{surfaces[s].p3, c, b});
            faces2.push_back(SURFACE{a, b, c});
        }

        surfaces.clear();
        for (int s = 0; s < (int)faces2.size(); s++)
        {
            surfaces.push_back(faces2[s]);
        }
    }

    // Copy surfaces into main model
    bool toggle;
    for (int i = 0; i < (int)surfaces.size(); i++)
    {
        // Fix the obj index start at 1 "bug"
        surfaces[i].p1++;
        surfaces[i].p2++;
        surfaces[i].p3++;

        if (true == toggle)
        {
            surfaces[i].colour = FLAT_WHITE;
            toggle = false;
        }
        else
        {
            surfaces[i].colour = FLAT_BLACK;
            toggle = true;
        }
    }

    // Correct size
    for (int i = 0; i < (int)data.size(); i++)
    {
        data[i].x *= radius;
        data[i].y *= radius;
        data[i].z *= radius;
    }

    // Other variables    
    numSurf = surfaces.size();
    numP = data.size();
    position = pos;

	Mesh_normalise();

    middlePointIndexCache.clear();

    printf("Icosphere :\r\nVertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", numP, numNorm, numSurf);
}

// EOF
