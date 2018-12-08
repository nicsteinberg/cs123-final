#ifndef CONE_H
#define CONE_H

#include "Shape.h"
#include "Cap.h"

/**
 * @class Cone
 *
 * Fits within a unit cube and is similar to the cylinder but with
 * the top (the end of the cylinder at Y = 0.5) pinched to a point.
 */
class Cone
    : public Shape
{

public:
    Cone(int p1, int p2);
    virtual ~Cone();

protected:
    void genVertices(int p1, int p2);
    virtual void draw() override;

    std::unique_ptr<Cap> m_cap;

    std::vector<float> convertCoords(float radius, float y, float theta);
    void makeTriangle(float radius, float y, float theta, float theta_seg, int p1);
    std::vector<float> makeNormal(float theta);
};

#endif // CONE_H
