#ifndef FACE_H
#define FACE_H

/** Contains three integers that refer to vertices to represent a face **/
struct Face {
    int v1;
    int v2;
    int v3;
    Face (int v1_, int v2_, int v3_)
        : v1(v1_), v2(v2_), v3(v3_) {};
};

#endif
