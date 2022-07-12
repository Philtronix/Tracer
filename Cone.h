#pragma once

#include "model.h"
#include <map>

class Cone : public Model
{
public:
    Cone() = delete;
    Cone(int height, int radius, int resolution, Vec3D pos);
    ~Cone();

private:
    int AddVertex(double x, double y, double z);
};
