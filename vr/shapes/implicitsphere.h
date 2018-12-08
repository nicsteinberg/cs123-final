#ifndef IMPLICITSPHERE_H
#define IMPLICITSPHERE_H

#include <glm.hpp>
#include "shapes/implicitshape.h"

class ImplicitSphere : public ImplicitShape
{
public:
    ImplicitSphere();
    float intersect(glm::vec4 p, glm::vec4 d) override;
    glm::vec4 getNormal(glm::vec4 p, glm::vec4 d, float t) override;
    glm::vec2 getTextureCoords(glm::vec4 p, glm::vec4 d, float t) override;
};

#endif // IMPLICITSPHERE_H
