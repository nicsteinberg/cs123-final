#include "implicitshape.h"

// Parent class for all implicit shapes for ray. Each has functionality
// for intersection t values and normals at given intersections.
ImplicitShape::ImplicitShape()
{
}

glm::vec4 ImplicitShape::getNormal(glm::vec4 p, glm::vec4 d, float t) {
    return glm::vec4();
}

glm::vec2 ImplicitShape::getTextureCoords(glm::vec4 p, glm::vec4 d, float t) {
    return glm::vec2();
}


