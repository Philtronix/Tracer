#pragma once

#include "model.h"
#include <map>

class Rectangle : public Model
{
public:
    Rectangle() = delete;
    Rectangle(int width, int height, int numRows, int numColumns, Vec3D pos);
    ~Rectangle();

private:
    int AddVertex(double x, double y, double z);
};
