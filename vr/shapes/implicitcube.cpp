#include "implicitcube.h"

#include <vector>
#include <iostream>

ImplicitCube::ImplicitCube()
{
    m_epsilon = pow(10, -4);
}

// For any given t and p,d vectors, if the resulting coordinates
// have any value outside range [-0.5, 0.5], set t equal to -1.
// Uses an epsilon value (10^-4) to prevent speckling in precision errors.
void ImplicitCube::checkForSideBounds(float *t, glm::vec4 p, glm::vec4 d) {

    glm::vec4 result = p + *t * d;

    float bound = 0.5 + m_epsilon;

    if (
            result.x > bound || result.x < -bound ||
            result.y > bound || result.y < -bound ||
            result.z > bound || result.z < -bound) {
        *t = -1;
    }
}

// Returns a positive t value if the given eye and direction vectors result in an intersection
// with any face of the unit cube. -1 if no intersection.
float ImplicitCube::intersect(glm::vec4 p, glm::vec4 d) {

    std::vector<float> t_for_each_side;
    t_for_each_side.resize(6);

    // Get t value for each of the six sides.
    t_for_each_side[0] = (+0.5 - p.x) / d.x;
    t_for_each_side[1] = (-0.5 - p.x) / d.x;
    t_for_each_side[2] = (+0.5 - p.y) / d.y;
    t_for_each_side[3] = (-0.5 - p.y) / d.y;
    t_for_each_side[4] = (+0.5 - p.z) / d.z;
    t_for_each_side[5] = (-0.5 - p.z) / d.z;

    // Check each t for being within cube bounds.
    for (unsigned int i = 0; i < t_for_each_side.size(); i++) {
        checkForSideBounds(&t_for_each_side[i], p, d);
    }

    // Find and return the minimum of the t's.
    float max = std::numeric_limits<float>::max();
    float t_min = max;
    for (unsigned int i = 0; i < t_for_each_side.size(); i++) {
        float t = t_for_each_side[i];
        if (t >= 0 && t < t_min) {
            t_min = t;
        }
    }

    if (max - t_min < m_epsilon) {
        return -1;
    }

    return t_min;

}

// Return the object-space normal at the intersection given in arguments.
glm::vec4 ImplicitCube::getNormal(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;

    float bound = 0.5;

    if (location.x - bound < m_epsilon && bound - location.x < m_epsilon) {
        return glm::vec4(1.f, 0.f, 0.f, 0.f);
    } else if (bound + location.x > - m_epsilon && bound + location.x < m_epsilon) {
        return glm::vec4(-1.f, 0.f, 0.f, 0.f);
    } else  if (location.y - bound < m_epsilon && bound - location.y < m_epsilon) {
        return glm::vec4(0.f, 1.f, 0.f, 0.f);
    } else if (bound + location.y > - m_epsilon && bound + location.y < m_epsilon) {
        return glm::vec4(0.f, -1.f, 0.f, 0.f);
    } else  if (location.z - bound < m_epsilon && bound - location.z < m_epsilon) {
        return glm::vec4(0.f, 0.f, 1.f, 0.f);
    } else if (bound + location.z > - m_epsilon && bound + location.z < m_epsilon) {
        return glm::vec4(0.f, 0.f, -1.f, 0.f);
    } else {
        return glm::vec4(1.f, 1.f, 1.f, 0.f);
    }

}

glm::vec2 ImplicitCube::getTextureCoords(glm::vec4 p, glm::vec4 d, float t) {

    glm::vec4 location = p + t * d;
    glm::vec2 flat_location;
    float bound = 0.5;
    float epsilon = pow(10, -4);
    if (location.x - bound < epsilon && bound - location.x < epsilon) {
        flat_location = glm::vec2(-location.z, -location.y);
    } else if (bound + location.x > - epsilon && bound + location.x < epsilon) {
        flat_location = glm::vec2(location.z, -location.y);
    } else  if (location.y - bound < epsilon && bound - location.y < epsilon) {
        flat_location = glm::vec2(location.x, location.z);
    } else if (bound + location.y > - epsilon && bound + location.y < epsilon) {
        flat_location = glm::vec2(location.x, -location.z);
    } else  if (location.z - bound < epsilon && bound - location.z < epsilon) {
        flat_location = glm::vec2(location.x, -location.y);
    } else if (bound + location.z > - epsilon && bound + location.z < epsilon) {
        flat_location = glm::vec2(-location.x, -location.y);
    } else {
        flat_location = glm::vec2(location.x, location.z);
    }

    flat_location += 0.5;
    return flat_location;

}
