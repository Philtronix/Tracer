#include "Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "model.h"
#include <map>

// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

extern Model model;
Model sphere;

// Create a map that stores strings indexed by strings
std::map<int, int> middlePointIndexCache;

// Add vertex to mesh, fix position to be on unit sphere, return index
int AddVertex(double x, double y, double z)
{
    double length = sqrt((x * x) + (y * y) + (z * z));
    double vx = x / length;
    double vy = y / length;
    double vz = z / length;

    sphere.data.push_back(Vec3D(vx, vy, vz));           // Model
    sphere.normal.push_back(Vec3D(vx, vy, vz));         // Model
    sphere.tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    sphere.showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    sphere.numP++;

    return (sphere.numP - 1);
}

// Return index of point in the middle of p1 and p2
int GetMiddlePoint(int p1, int p2)
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
    Vec3D point1 = sphere.data[p1];
    Vec3D point2 = sphere.data[p2];
    Vec3D middle = Vec3D((point1.x + point2.x) / 2.0, 
                         (point1.y + point2.y) / 2.0, 
                         (point1.z + point2.z) / 2.0);

    // add vertex makes sure point is on unit sphere
    int i = AddVertex(middle.x, middle.y, middle.z); 

    // Store it, return index
    middlePointIndexCache.insert({key, i});
    return i;
}

void Create(int recursionLevel)
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
    sphere.surfaces.push_back(SURFACE{0, 11,  5});
    sphere.surfaces.push_back(SURFACE{0,  5,  1});
    sphere.surfaces.push_back(SURFACE{0,  1,  7});
    sphere.surfaces.push_back(SURFACE{0,  7, 10});
    sphere.surfaces.push_back(SURFACE{0, 10, 11});

    // 5 adjacent faces 
    sphere.surfaces.push_back(SURFACE{ 1,  5, 9});
    sphere.surfaces.push_back(SURFACE{ 5, 11, 4});
    sphere.surfaces.push_back(SURFACE{11, 10, 2});
    sphere.surfaces.push_back(SURFACE{10,  7, 6});
    sphere.surfaces.push_back(SURFACE{ 7,  1, 8});

    // 5 faces around point 3
    sphere.surfaces.push_back(SURFACE{3, 9, 4});
    sphere.surfaces.push_back(SURFACE{3, 4, 2});
    sphere.surfaces.push_back(SURFACE{3, 2, 6});
    sphere.surfaces.push_back(SURFACE{3, 6, 8});
    sphere.surfaces.push_back(SURFACE{3, 8, 9});

    // 5 adjacent faces 
    sphere.surfaces.push_back(SURFACE{4, 9,  5});
    sphere.surfaces.push_back(SURFACE{2, 4, 11});
    sphere.surfaces.push_back(SURFACE{6, 2, 10});
    sphere.surfaces.push_back(SURFACE{8, 6,  7});
    sphere.surfaces.push_back(SURFACE{9, 8,  1});

    std::vector<SURFACE> faces2;

    // Refine triangles
    for (int i = 0; i < recursionLevel; i++)
    {
        faces2.clear();
        for (int s = 0; s < (int) sphere.surfaces.size(); s++)
        {
            // Replace triangle by 4 triangles
            // Creates or finds the point and returns its index
            a = GetMiddlePoint(sphere.surfaces[s].p1, sphere.surfaces[s].p2);
            b = GetMiddlePoint(sphere.surfaces[s].p2, sphere.surfaces[s].p3);
            c = GetMiddlePoint(sphere.surfaces[s].p3, sphere.surfaces[s].p1);

            faces2.push_back(SURFACE{sphere.surfaces[s].p1, a, c});
            faces2.push_back(SURFACE{sphere.surfaces[s].p2, b, a});
            faces2.push_back(SURFACE{sphere.surfaces[s].p3, c, b});
            faces2.push_back(SURFACE{a, b, c});
        }

        sphere.surfaces.clear();
        for (int s = 0; s < (int)faces2.size(); s++)
        {
            sphere.surfaces.push_back(faces2[s]);
        }
    }

    // Copy surfaces into main model
    for (int i = 0; i < (int)sphere.surfaces.size(); i++)
    {
        model.surfaces.push_back(sphere.surfaces[i]);

        // Fix the obj index start at 1 "bug"
        model.surfaces[i].p1++;
        model.surfaces[i].p2++;
        model.surfaces[i].p3++;
    }
    model.numSurf += sphere.surfaces.size();
    
    // Copy points into main model
    for (int i = 0; i < (int)sphere.data.size(); i++)
    {
        model.data.push_back(sphere.data[i]);
        model.tmp.push_back(sphere.data[i]);
    }
    model.numP += sphere.data.size();

    // Expand size
    // Rename this "radius" ?
    model.zoom = 200;
    for (int i = 0; i < (int)sphere.data.size(); i++)
    {
        model.data[i].x *= model.zoom;
        model.data[i].y *= model.zoom;
        model.data[i].z *= model.zoom;
    }
    
    printf("Vertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", model.numP, model.numNorm, model.numSurf);
}

// EOF
