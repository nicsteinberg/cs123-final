#ifndef CAP_H
#define CAP_H

#include "Shape.h"

/**
 * @class Cap
 *
 * Bases for a) a cone, and b) a cylinder.
 *
 * Lies on the planes y += 0.5 and has the equation x^2 + z^2 = (0.5)^2.
 * Can use x = rcos(theta) and z = rsin(theta) to determine x and z
 * values of the points on the circle. r = 0.5 and angle theta differs
 * for each vertex. Divide full circle by p2 to determine spacing between each theta.
 */
class Cap
    : public Shape
{

public:
    Cap(int p1, int p2, float y);
    virtual ~Cap();

protected:
    void genVertices(int p1, int p2, float y);

    std::vector<float> convertCoords(float radius, float y, float theta);
    void makeTriangle(float radius, float y, float theta, float theta_seg, std::vector<float> &normal, int p1);
};

#endif // CAP_H
