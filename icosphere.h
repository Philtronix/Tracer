#pragma once

#include "model.h"
#include <map>

class Icosphere : public Model
{
public:
    Icosphere() = delete;
    Icosphere(int radius, int recursionLevel, Vec3D pos);
    ~Icosphere();

private:
    int AddVertex(double x, double y, double z);
    int GetMiddlePoint(int p1, int p2);

    // Create a map that stores strings indexed by strings
    std::map<int, int> middlePointIndexCache;

};
