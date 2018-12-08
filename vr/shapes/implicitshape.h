#ifndef IMPLICITSHAPE_H
#define IMPLICITSHAPE_H

#include "glm.hpp"
#include <math.h>

class ImplicitShape
{
public:
    ImplicitShape();
    virtual float intersect(glm::vec4 p, glm::vec4 d) {
        return 0;
    }
    virtual glm::vec4 getNormal(glm::vec4 p, glm::vec4 d, float t);
    virtual glm::vec2 getTextureCoords(glm::vec4 p, glm::vec4 d, float t);
};

#endif // IMPLICITSHAPE_H
