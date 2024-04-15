#include "../Vec3D.h"
#include <sys/types.h>
#include <math.h>
#include "Rectangle.h"


// Add vertex to mesh, fix position to be on unit sphere, return index
int Rectangle::AddVertex(double x, double y, double z)
{
    data.push_back(Vec3D(x, y, z));              // Model
    tmp.push_back(Vec3D(0.0, 0.0, 0.0));         // Model
    normal.push_back(Vec3D(0.0, 0.0, 0.0));      // Model
    tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));     // Rotated
    showNorm.push_back(Vec3D(0.0, 0.0, 0.0));    // Rotated & stretched

    numP++;

    return (numP - 1);
}

Rectangle::~Rectangle()
{

}

Rectangle::Rectangle(int width, int height, int numRows, int numColumns, Vec3D pos)
{
    double    x = 0.0;
    double    y = 0.0;
    double    z = 0.0;
    double    dX = 0.0;
    double    dY = 0.0;
    double    sX = 0.0;
    double    sY = 0.0;
    int       p1;
    int       p2;
    int       p3;
    int       g = 0;
    bool      toggle_r = false;
    bool      toggle_c = false;
    ColourRef white = FLAT_WHITE;
    ColourRef black = FLAT_BLACK;
    int       s = 0;

    dX = width / numColumns;
    dY = height / numRows;

    sX = 0.0 - (width / 2);
    sY = 0.0 - (height / 2);

    y = sY;
    // Create points
    for (int r = 0; r <= numRows; r++)
    {
        x = sX;
        for (int c = 0; c <= numColumns; c++)
        {
            AddVertex(x,  y,  z);
            x += dX;
        }
        y += dY;
    }

    // 3 rows, 3 columns
    // 0 1 2 3 
    // 4 5 6 7
    // 8 9 A B
    // C D E F

    // Create surfaces
    for (int r = 0; r < numRows; r++)
    {
        if (true == toggle_r)
        {
            toggle_r = false;
            toggle_c = false;
        }
        else
        {
            toggle_r = true;
            toggle_c = true;
        }

        for (int c = 0; c < numColumns; c++)
        {
            p1 = g + c;
            p2 = p1 + 1;
            p3 = p2 + numColumns;
            p1++;
            p2++;
            p3++;
            surfaces.push_back(SURFACE{p1, p2, p3});
            if (true == toggle_c)
            {
                surfaces[s].colour = white;
            }
            else
            {
                surfaces[s].colour = black;
            }
            s++;

            p1 = g + c + 1;
            p2 = p1 + numColumns + 1;
            p3 = p1 + numColumns;
            p1++;
            p2++;
            p3++;
            surfaces.push_back(SURFACE{p1, p2, p3});
            if (true == toggle_c)
            {
                surfaces[s].colour = white;
            }
            else
            {
                surfaces[s].colour = black;
            }
            s++;

            if (true == toggle_c)
            {
                toggle_c = false;
            }
            else
            {
                toggle_c = true;
            }
        }
        g += numColumns + 1;
    }

    // Other variables    
    numSurf = numRows * numColumns * 2;
    numP = (numRows + 1) * (numColumns + 1);
    position = pos;

	Mesh_normalise();

    printf("Rectangle :\r\nVertex   = %d\r\nNormals  = %d\r\nSurfaces = %d\r\n", numP, numNorm, numSurf);
}

// EOF
