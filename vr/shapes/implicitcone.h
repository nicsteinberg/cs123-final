#ifndef IMPLICITCONE_H
#define IMPLICITCONE_H

#include <glm.hpp>
#include "shapes/implicitshape.h"

class ImplicitCone : public ImplicitShape
{
public:
    ImplicitCone();
    float intersect(glm::vec4 p, glm::vec4 d) override;
    glm::vec4 getNormal(glm::vec4 p, glm::vec4 d, float t) override;
    glm::vec2 getTextureCoords(glm::vec4 p, glm::vec4 d, float t) override;
protected:
    float m_epsilon;
};

#endif // IMPLICITCONE_H
