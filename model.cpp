#include "model.h"

#include <string.h>

Model::Model()
{
	numP = 0;
	numSurf = 0;
	numNorm = 0;
	zoom = 1.0;
	objColour.red   = FLAT_RED;
	objColour.green = FLAT_GREEN;
	objColour.blue  = FLAT_BLUE;
}

Model::~Model()
{
	//for (int i = 0; i < numP; i++)
	//{
	//	delete data[i];
	//	delete tmp[i];
	//}
}

bool Model::LoadObjFile(char *path, char *file, double modelZoom)
{
	// Load index file
	FILE	*fp;
	char	line[1000];
	int		nBytes;
	int		nLines = 0;
	double	x;
	double	y;
	double	z;
	int		v1, v2, v3, v4;
	int		t1, t2, t3, t4;
	int		n1, n2, n3, n4;
	int		np;
	int		nn;
	int		ns;
	SURFACE surface;
	char	szModel[500];

	zoom = modelZoom;
	sprintf(szModel, "%s/%s", path, file);

	printf("Load : %s\r\n", file);

	fp = fopen(szModel, "rb");
	if (fp == NULL)
	{
		printf(line, "Failed to load file : %s", file);
		return false;
	}

	np = 0;
	ns = 0;
	nn = 0;
	while (ReadLine(line, &nBytes, fp) >= 0 || nBytes > 0)
	{
		nLines++;
		//printf("line [%s] %d\r\n", line, nBytes);

		if ('v' == line[0] && ' ' == line[1])
		{
			// v  21.0000 36.0000 -0.0000
			if (3 == sscanf(line, "v  %lf %lf %lf", &x, &y, &z))
			{
				// Vector
				x *= zoom;
				y *= zoom;
				z *= zoom;
				data.push_back(Vec3D(x, y, z));
				tmp.push_back(Vec3D(x, y, z));
				np++;
			}
			else
			{
				printf("Bad vector [%s]\r\n", line);
			}
		}
		else if ('v' == line[0] && 'n' == line[1])
		{
			// vector normal
			// vn  21.0000 36.0000 -0.0000
			if (3 == sscanf(line, "vn  %lf %lf %lf", &x, &y, &z))
			{
				//printf("vn 3 : %f %f %f\r\n", x, y, z);
				normal.push_back(Vec3D(x, y, z));
				tmpNorm.push_back(Vec3D(x, y, z));
				showNorm.push_back(Vec3D(x, y, z));

				nn++;
			}
			else
			{
				printf("Bad vector normal : [%s]\r\n", line);
			}
		}
		else if ('v' == line[0] && 't' == line[1])
		{
			// Texture co'ordinates
			printf("ignorred [%s]\r\n", line);
		}
		else if ('f' == line[0] && ' ' == line[1])
		{
			// surfaces
			// f 230/113/231 263/110/264 231/108/232
			// f 263/110/264 230/113/231 233/130/234 264/127/265
			if (12 == sscanf(line, "f  %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
									&v1, &t1, &n1, 
									&v2, &t2, &n2, 
									&v3, &t3, &n3,
									&v4, &t4, &n4))
			{
				// Convert 4 point surface into two three point surfaces
				surface.p1 = v1;
				surface.p2 = v2;
				surface.p3 = v3;
				surface.t1 = t1;
				surface.t2 = t2;
				surface.t3 = t3;
				surface.n1 = n1;
				surface.n2 = n2;
				surface.n3 = n3;
				surfaces.push_back(surface);
				ns++;

				surface.p1 = v3;
				surface.p2 = v4;
				surface.p3 = v1;
				surface.t1 = t3;
				surface.t2 = t4;
				surface.t3 = t1;
				surface.n1 = n3;
				surface.n2 = n4;
				surface.n3 = n1;
				surfaces.push_back(surface);
				ns++;
				printf("f12 : %d %d %d\r\n", v1, v2, v3);
			}
			else if (9 == sscanf(line, "f  %d/%d/%d %d/%d/%d %d/%d/%d", 
										&v1, &t1, &n1, 
										&v2, &t2, &n2, 
										&v3, &t3, &n3))
			{
				surface.p1 = v1;
				surface.p2 = v2;
				surface.p3 = v3;
				surface.t1 = t1;
				surface.t2 = t2;
				surface.t3 = t3;
				surface.n1 = n1;
				surface.n2 = n2;
				surface.n3 = n3;
				surfaces.push_back(surface);
				ns++;
				//printf("f9 : %d %d %d\r\n", v1, v2, v3);
			}
			else if (6 == sscanf(line, "f  %d//%d %d//%d %d//%d", 
										&v1, &n1, 
										&v2, &n2, 
										&v3, &n3))
			{
				surface.p1 = v1;
				surface.p2 = v2;
				surface.p3 = v3;
				surface.n1 = n1;
				surface.n2 = n2;
				surface.n3 = n3;
				surfaces.push_back(surface);
				ns++;
				//printf("f6 : %d %d %d\r\n", v1, v2, v3);
			}
			else if (3 == sscanf(line, "f %d %d %d", &v1, &v2, &v3))
			{
				surface.p1 = v1;
				surface.p2 = v2;
				surface.p3 = v3;
				surfaces.push_back(surface);
				ns++;
				//printf("f3 : %d %d %d (%d)\r\n", v1, v2, v3, nLines);
			}
			else
			{
				printf("Bad surface [%s] %d\r\n", line, nBytes);
			}
		}
		else if ('#' == line[0])
		{
			// Comment
			printf("Comment [%s]\r\n", line);
		}
		else if (0 == strncmp(line, "mtllib", 6))
		{
			printf("mtllib [%s]\r\n", line);
		}
		else if (0 == strncmp(line, "usemtl", 6))
		{
			printf("usemtl [%s]\r\n", line);
		}
		else if (0 == strncmp(line, "s off", 5))
		{
			printf("s off [%s]\r\n", line);
		}
		else
		{
			printf("Bad line [%s] line %d, %d bytes\r\n", line, nLines, nBytes);
		}
	}
	fclose(fp);
	printf("Load : %s [done]\r\n", file);

	numP = np;
	numSurf = ns;
	numNorm = nn;

	if (0 == nn)
	{
		printf("No normals found, calculating\r\n");
		Mesh_normalise();
	}

	printf("Vertex   = %d\r\nNormals  = %d\r\nLines    = %d\r\nSurfaces = %d\r\n", numP, numNorm, nLines, numSurf);

	return true;
}

Vec3D Model::Vector_CrossProduct(Vec3D &v1, Vec3D &v2)
{
	// Nx = Ay * Bz - Az * By
	// Ny = Az * Bx - Ax * Bz
	// Nz = Ax * By - Ay * Bx

	Vec3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

// https://iquilezles.org/articles/normals/
void Model::Mesh_normalise()
{
	int	  i;
	int   ia;
	int   ib;
	int   ic;
	Vec3D e1; 
	Vec3D e2; 
	Vec3D no;

	printf("Mesh_normalise()\r\n");

	// Create empty normal vectors
    for (int i = 0; i < numP; i++ )
	{
		normal.push_back(Vec3D(0.0, 0.0, 0.0));		// Model
		tmpNorm.push_back(Vec3D(0.0, 0.0, 0.0));	// Rotated
		showNorm.push_back(Vec3D(0.0, 0.0, 0.0));	// Rotated & stretched
	}

	// Scan through each surface and calculate it's normal
    for (i = 0; i < numSurf; i++)
    {
		// Which points make up this surface
		ia = surfaces[i].p1;
		ib = surfaces[i].p2;
		ic = surfaces[i].p3;

		surfaces[i].n1 = ia;
		surfaces[i].n2 = ib;
		surfaces[i].n3 = ic;

		// Object index "bug" (starts at 1)
		ia--;
		ib--;
		ic--;

		// Work out the surface normal
		e1 = data[ib] - data[ia]; 
		e2 = data[ic] - data[ia]; 

		// So for a triangle p1, p2, p3, if the vector 
		// A = p2 - p1 and the vector 
		// B = p3 - p1 then the normal 
		// N = A x B and can be calculated by:
		// Nx = Ay * Bz - Az * By
		// Ny = Az * Bx - Ax * Bz
		// Nz = Ax * By - Ay * Bx
		no = Vector_CrossProduct(e1, e2);
		
		// Vertex normal = sum of all surfaces it's in
		normal[ia].x += no.x;
		normal[ia].y += no.y;
		normal[ia].z += no.z;

		normal[ib].x += no.x;
		normal[ib].y += no.y;
		normal[ib].z += no.z;

		normal[ic].x += no.x;
		normal[ic].y += no.y;
		normal[ic].z += no.z;
    }

	// Normalise each vertex normal
    for(i = 0; i < numP; i++)
	{
		normal[i].normalise();
	}

	numNorm = numP;
	printf("Mesh_normalise() - [done]\r\n");
}

// Returns -1 on EOF
int Model::ReadLine(char *dst, int *nBytes, FILE *fp)
{
	char	c;
	int		iLen = 0;
	int		num = 1;

	while (num > 0)
	{
		// Read in one byte
		num = fread(&c, 1, 1, fp);
		if (num > 0)
		{
			if ('\r' == c)
			{
				num = fread(&c, 1, 1, fp);
				if (0 == num)
				{
					num = -1;
				}
				else 
				{
					if (c != '\n')
					{
						fseek(fp, -1, SEEK_CUR);
					}
					num = 0;
				}
			}
			else if ('\n' == c)
			{
				num = fread(&c, 1, 1, fp);
				if (0 == num)
				{
					num = -1;
				}
				else 
				{
					if (c != '\r')
					{
						fseek(fp, -1, SEEK_CUR);
					}
					num = 0;
				}
			}
			else
			{
				dst[iLen++] = c;
			}
		}
		else
		{
			num = -1;
		}
	}

	*nBytes = iLen;

	dst[iLen] = 0;

	return num;
}

void Model::SetColour(ColourRef colour)
{
    objColour = colour;
}

// EOF
