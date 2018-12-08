#include "implicitcylinder.h"

#include <iostream>

ImplicitCylinder::ImplicitCylinder()
{
    m_epsilon = pow(10, -4);
}

// Returns a positive t value if the given eye and direction vectors result in an intersection
// with the barrel of the unit cylinder. -1 if no intersection.
float intersectBarrel(glm::vec4 p, glm::vec4 d) {

    // Quadratic equation.
    float a = d.x * d.x + d.z * d.z;
    float b = 2 * p.x * d.x + 2 * p.z * d.z;
    float c = p.x * p.x + p.z * p.z - 0.25;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1;
    }

    // Get both resulting t values.
    float t1 = (-1 * b + sqrt(discriminant)) / (2 * a);
    float t2 = (-1 * b - sqrt(discriminant)) / (2 * a);

    glm::vec4 result1 = p + t1 * d;
    glm::vec4 result2 = p + t2 * d;

    // Throw out any results outside height bounds.
    float top = 0.5;
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
// with either base of the unit cylinder. -1 if no intersection.
float intersectBase(glm::vec4 p, glm::vec4 d) {

    // Get t values for both caps.
    float t1 = (-0.5 - p.y) / d.y;
    float t2 = (+0.5 - p.y) / d.y;

    glm::vec4 result1 = p + t1 * d;
    glm::vec4 result2 = p + t2 * d;

    // Toss out any out-of-bounds results.
    if (result1.x * result1.x + result1.z * result1.z > 0.25) {
        t1 = -1;
    }
    if (result2.x * result2.x + result2.z * result2.z > 0.25) {
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
// with the unit cylinder. -1 if no intersection.
float ImplicitCylinder::intersect(glm::vec4 p, glm::vec4 d) {

    float barrel = intersectBarrel(p, d);
    float base = intersectBase(p, d);

    if (barrel < 0) {
        return base;
    } else if (base < 0) {
        return barrel;
    } else {
        return glm::min(barrel, base);
    }
}

// Return the object-space normal at the intersection given in arguments.
glm::vec4 ImplicitCylinder::getNormal(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;

    // Either cap:
    float bound = 0.5;
    if (abs(location.y - bound) < m_epsilon) {
        return glm::vec4(0.f, 1.f, 0.f, 0.f);
    } else if (abs(location.y + bound) < m_epsilon) {
        return glm::vec4(0.f, -1.f, 0.f, 0.f);
    }

    // Return normalized (x, 0, z) component for the barrel.
    glm::vec4 normal = glm::vec4(location.x, 0.f, location.z, 0.f);
    return glm::normalize(normal);

}

glm::vec2 ImplicitCylinder::getTextureCoords(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;
    float bound = 0.5;
    float epsilon = pow(10, -4);

    if (location.y - bound < epsilon && bound - location.y < epsilon) {
        return glm::vec2(location.x, location.z) + bound;
    } else if (location.y + bound > - epsilon && bound + location.y < epsilon) {
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
