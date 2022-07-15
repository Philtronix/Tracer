# Tracer
Raytracer proof of concept. No actual ray tracing yet, just displays obj models and primitives.

Messy code, it works but needs cleaning and optimising.

Based on code taken from :

http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html<br>
https://iquilezles.org/articles/normals/<br>
http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html<br>
http://en.wikipedia.org/wiki/Slope<br>
https://www.danielsieger.com/blog/2021/05/03/generating-primitive-shapes.html<br>

GTK:
Treeview : https://www.cs.uni.edu/~okane/Code/Glade%20Cookbook/

TODO :
1. Delete Gouraud and rename DrawPhil.cpp .h - [done]
2. Make icosphere into class based on model - [done]
3. Add support for mulyiple models - [done]
4. Optimise, points normals etc.
5. Move vector maths to Vec3D.cpp, dot etc
6. Add Phong
7. Z buffer gouraud (leave fill on sort) - [done]

BUGS :
1. Low poly icosphere shows odd effects
2. Monkey.obj in Gouraud shows "RED" errors
3. Teapot_2.obj in Gouraud, pitch = 0 shows black lines on shading
4. Cube shows Gouraud error really well
