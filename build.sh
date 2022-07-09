#!/bin/bash

#gcc -Wno-format -o tracer tracer.c -rdynamic -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
#g++ -Wno-format -export-dynamic -o tracer tracer.cpp -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

#g++ -rdynamic -o tracer -Wall -g tracer.cpp $(pkg-config --cflags --libs gtk+-3.0 gmodule-2.0 )
g++ -rdynamic -o tracer -Wall -g \
    tracer.cpp \
    model.cpp \
    Matrix4.cpp \
    Vec3D.cpp \
    Views/DrawPoints.cpp \
    Views/DrawWire.cpp \
    Views/DrawFlat.cpp \
    Views/DrawGouraud.cpp \
    Views/DrawPhil.cpp \
    icosphere.cpp \
    Rectangle.cpp \
    cube.cpp \
    $(pkg-config --cflags --libs gtk+-3.0 gmodule-2.0 )

