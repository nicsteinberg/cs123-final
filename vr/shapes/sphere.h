#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"

/**
 * @class Sphere
 *
 * Centered at the origin, and has a radius of 0.5.
 */
class Sphere
    : public Shape
{

public:
    Sphere(int p1, int p2);
    virtual ~Sphere();

protected:
    void genVertices(int p1, int p2);
    void makeSide(float radius, float theta_seg, float psi_seg, int p1, int p2);
    std::vector<float> makeNormal(std::vector<float> point);
    std::vector<float> convertCoords(float radius, float theta, float psi);

    void getUV(std::vector<float> intersection);
};

#endif // SPHERE_H
