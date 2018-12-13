#ifndef CYLINDER_H
#define CYLINDER_H

#include "shape.h"
#include "Cap.h"

/**
 * @class Cylinder
 *
 * Has height of one unit, and is one unit in diameter. The Y axis
 * passes vertically through the center; the caps are parallel to
 * the XZ plane. So the bounds are -0.5 to 0.5 along all axes.
 */
class Cylinder
    : public Shape
{

public:
    Cylinder(int p1, int p2);
    virtual ~Cylinder();

protected:
    void genVertices(int p1, int p2);
    virtual void draw() override;

    std::unique_ptr<Cap> m_cap;
    std::unique_ptr<Cap> m_cap2;

    std::vector<float> convertCoords(float radius, float y, float theta);
    std::vector<float> makeNormal(float theta);
    void makeRectangle(float radius, float y, float theta, float theta_seg, int p1);

    void getUV(std::vector<float> intersection);
};

#endif // CYLINDER_H
