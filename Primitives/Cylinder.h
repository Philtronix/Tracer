#pragma once

#include "../model.h"
#include <map>

class Cylinder : public Model
{
public:
    Cylinder() = delete;
    Cylinder(int height, int radius, int resolution, Vec3D pos);
    ~Cylinder();

private:
    int AddVertex(double x, double y, double z);
};
