#pragma once

#include "model.h"
#include <map>

class Torus : public Model
{
public:
    Torus() = delete;
    Torus(int radial_resolution, int tubular_resolution, double radius, double thickness, Vec3D pos);

    ~Torus();

private:
    int AddVertex(double x, double y, double z);
};
