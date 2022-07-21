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
1. Surfaces have individual colours - [done]
2. Activate menu - File : Quit - [done]
3. File : load - [done]
4. Optimise, points normals etc.
5. Move vector maths to Vec3D.cpp, dot etc
6. Add Phong
7. Add primitives menu
8. Individuaal primitives dialog boxes
9. Editiable primitives values

https://www.braynzarsoft.net/viewtutorial/q16390-transformations-and-world-view-projection-space-matrices
Add a world matrix to each model to transform from object to world.
  Rotate
  Scale
  Translate
Add View matrix (Camera)
Add Projection matrix
finalvertex.pos = vertex.pos * worldMatrix * viewMatrix * projectionMatrix;


Left Handed Coordinate System
The left handed coordinate system is when the positive y axis is pointing up, the positive x axis is pointing right, and the positive z axis is pointing forward.

Right Handed Coordinate System
The right handed coordinate system is when the positive y axis is pointing up, the positive x axis is pointing right, and the positive z axis is pointing towards you.
(use left)

BUGS :
1. Low poly icosphere shows odd effects
2. Monkey.obj in Gouraud shows "RED" errors
3. Teapot_2.obj in Gouraud, pitch = 0 shows black lines on shading
4. Cube shows Gouraud error really well
