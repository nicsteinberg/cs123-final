#ifndef IMPLICITCUBE_H
#define IMPLICITCUBE_H

#include <glm.hpp>
#include "shapes/implicitshape.h"

class ImplicitCube : public ImplicitShape
{
public:
    ImplicitCube();
    float intersect(glm::vec4 p, glm::vec4 d) override;
    glm::vec4 getNormal(glm::vec4 p, glm::vec4 d, float t) override;
    glm::vec2 getTextureCoords(glm::vec4 p, glm::vec4 d, float t) override;
protected:
    float m_epsilon;
    void checkForSideBounds(float *t, glm::vec4 p, glm::vec4 d);
};

#endif // IMPLICITCUBE_H
