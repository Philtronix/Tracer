# Tracer
Raytracer proof of concept

Messy code, it works but needs cleaning and optimising.

Based on code taken from :
http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
https://iquilezles.org/articles/normals/
http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
http://en.wikipedia.org/wiki/Slope

TODO :
1. Delete Gouraud and rename DrawPhil.cpp .h
2. Mske icosphere into class based on model
3. Add support for mulyiple models
4. Optimise, points normals etc.
5. Move vector maths to Vec3D.cpp
6. Add Phong
7. Z buffer gouraud (leave fill on sort)
