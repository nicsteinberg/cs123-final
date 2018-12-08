#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"

class Sphere : public Shape
{
public:
    Sphere(int shapeParameter1, int shapeParameter2);
    virtual ~Sphere();

protected:
    std::vector<float> makeFace();

    std::vector<float> coordinatesOnSphere(float radius, float theta, float phi);
    std::vector<float> doubleEachValue(std::vector<float> input);
    std::vector<float> makeVertices() override;
    std::vector<std::unique_ptr<OpenGLShape>> m_triangles;

};

#endif // SPHERE_H
