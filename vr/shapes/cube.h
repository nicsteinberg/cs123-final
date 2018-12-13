#ifndef CUBE_H
#define CUBE_H

#include "shape.h"
#include <string>

/**
 * @class Cube
 *
 * Has unit length edges from -0.5 to 0.5 along all axes.
 */
class Cube
    : public Shape
{

public:
    Cube(int p1);
    virtual ~Cube();

protected:
    void genVertices(int p1);
    void makeFace(int p1, float x, float y, float z, int face);
    void addNormal(int face);
    void addVertex(float x, float y, float z);
    void addUV(int vertex);

    void addUV(float x, float y, float z);

    void addUV(int face, int vertex);
};

#endif // CUBE_H
