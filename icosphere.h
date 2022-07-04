#pragma once

void Create(int recursionLevel);

class Icosphere
{
public:
    Icosphere() = delete;
    Icosphere(int radius, int recursionLevel);
    ~Icosphere();

private:
    int AddVertex(double x, double y, double z);
    int GetMiddlePoint(int p1, int p2);
//    void Create(int recursionLevel);
};
