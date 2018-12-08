#include "implicitsphere.h"

ImplicitSphere::ImplicitSphere() : ImplicitShape()
{
}

// Returns a positive t value if the given eye and direction vectors result in an intersection
// with the unit sphere. -1 if no intersection.
float ImplicitSphere::intersect(glm::vec4 p, glm::vec4 d) {

    // Quadratic equation.
    float a = d.x * d.x + d.y * d.y + d.z * d.z;
    float b = 2 * p.x * d.x + 2 * p.y * d.y + 2 * p.z * d.z;
    float c = p.x * p.x + p.y * p.y + p.z * p.z - 0.25;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1;
    }

    // Get the resulting t values.
    float t1 = (-1 * b + sqrt(discriminant)) / (2 * a);
    float t2 = (-1 * b - sqrt(discriminant)) / (2 * a);

    // Return the minimum positive t.
    if (t1 > 0 && t2 > 0) {
        return glm::min(t1, t2);
    } else if (t1 > 0) {
        return t1;
    } else if (t2 > 0) {
        return t2;
    } else {
        return -1;
    }
}

// Return the object-space normal at the intersection given in arguments.
glm::vec4 ImplicitSphere::getNormal(glm::vec4 p, glm::vec4 d, float t) {

    // Double the object-space intersection to get the corresponding normal.
    glm::vec4 location = p + t * d;
    float x = location.x;
    float y = location.y;
    float z = location.z;
    return glm::normalize(glm::vec4(2 * x, 2 * y, 2 * z, 0.f));

}

glm::vec2 ImplicitSphere::getTextureCoords(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;
    float bound = 0.5;
    float epsilon = pow(10, -4);

    if (location.y - bound < epsilon && bound - location.y < epsilon) {
        return glm::vec2(0.5, 1.f) + bound;
    } else if (location.y + bound > - epsilon && bound + location.y < epsilon) {
        return glm::vec2(0.5, 0.f) + bound;
    }

    float theta = atan2(location.z, location.x);
    float u;
    if (theta < 0) {
        u = -theta / 2 / M_PI;
    } else {
        u = 1 - theta / 2 / M_PI;
    }

    float phi = asin(location.y / 0.5);
    float v = phi / M_PI + 0.5;

    return glm::vec2(u, v);

}
