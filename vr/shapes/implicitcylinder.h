#ifndef IMPLICITCYLINDER_H
#define IMPLICITCYLINDER_H

#include "shapes/implicitshape.h"

class ImplicitCylinder : public ImplicitShape
{
public:
    ImplicitCylinder();
    float intersect(glm::vec4 p, glm::vec4 d) override;
    glm::vec4 getNormal(glm::vec4 p, glm::vec4 d, float t) override;
    glm::vec2 getTextureCoords(glm::vec4 p, glm::vec4 d, float t) override;
protected:
    float m_epsilon;
};

#endif // IMPLICITCYLINDER_H
