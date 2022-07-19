#pragma once

#include "../model.h"
#include <map>

class Cube : public Model
{
public:
    Cube() = delete;
    Cube(int length, int width, int height, Vec3D pos);
    ~Cube();

private:
    int AddVertex(double x, double y, double z);
};
