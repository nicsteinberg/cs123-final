#include "implicitcone.h"

#include <iostream>

// Class with implicit equation calculations for a unit cone.
ImplicitCone::ImplicitCone()
 : ImplicitShape()
{
    m_epsilon = pow(10, -4);
}

// Returns a positive t value if the given eye and direction vectors result in an intersection
// with the body of the unit cone. -1 if no intersection.
float intersectBody(glm::vec4 p, glm::vec4 d) {

    // Quadratic equation.
    float a = d.x * d.x + d.z * d.z - 0.25 * d.y * d.y;
    float b = 2 * p.x * d.x + 2 * p.z * d.z - 0.5 * p.y * d.y + 0.25 * d.y;
    float c = p.x * p.x + p.z * p.z - 0.25 * p.y * p.y + 0.25 * p.y - 1.f / 16.f;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1;
    }

    // Get both possible t values.
    float t1 = (-1 * b + sqrt(discriminant)) / (2 * a);
    float t2 = (-1 * b - sqrt(discriminant)) / (2 * a);

    glm::vec4 result1 = p + t1 * d;
    glm::vec4 result2 = p + t2 * d;

    float top = 0.5;

    // Throw out any results outside height bounds.
    if (result1.y > top || result2.y < -top) {
        t1 = -1;
    }
    if (result2.y > top || result2.y < -top) {
        t2 = -1;
    }

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

// Returns a positive t value if the given eye and direction vectors result in an intersection
// with the cap of the unit cone. -1 if no intersection.=
float intersectCap(glm::vec4 p, glm::vec4 d) {

    float t = (-0.5 - p.y) / d.y;

    // Throw out any results outside bounds.
    glm::vec4 result = p + t * d;
    if (result.x * result.x + result.z * result.z > 0.25) {
        return -1;
    }

    return t;

}

// Returns a positive t value if the given eye and direction vectors result in an intersection
// with the unit cone. -1 if no intersection.
float ImplicitCone::intersect(glm::vec4 p, glm::vec4 d) {

    float body = intersectBody(p, d);
    float cap = intersectCap(p, d);

    if (cap < 0) {
        return body;
    } else if (body < 0) {
        return cap;
    } else {
        return glm::min(body, cap);
    }

}

// Return the object-space normal at the intersection given in arguments.
glm::vec4 ImplicitCone::getNormal(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;

    // Cap case:
    float bound = 0.5;
    if (abs(location.y + bound) < m_epsilon) {
        return glm::vec4(0.f, -1.f, 0.f, 0.f);
    }

    // Body case, given by gradient:
    float x = location.x;
    float z = location.z;
    glm::vec4 normal = glm::vec4(x, 0.5 * sqrt(x * x + z * z), z, 0.f);
    return glm::normalize(normal);

}

glm::vec2 ImplicitCone::getTextureCoords(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;
    float bound = 0.5;
    float epsilon = pow(10, -4);

    if (location.y + bound > - epsilon && bound + location.y < epsilon) {
        return glm::vec2(location.x, -location.z) + bound;
    }

    float theta = atan2(location.z, location.x);
    float u;
    if (theta < 0) {
        u = -theta / 2 / M_PI;
    } else {
        u = 1 - theta / 2 / M_PI;
    }

    float v = - location.y + bound;

    return glm::vec2(u, v);
}
