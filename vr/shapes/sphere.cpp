/**
 * @file   Sphere.cpp
 *
 * Implementation of a sphere.
 */

#include "sphere.h"
#include <cmath>
#include "gl/shaders/ShaderAttribLocations.h"
#include <iostream>

Sphere::Sphere(int p1, int p2)
    : Shape()
{
    // There must be at least three "sides" to the sphere.
    if (p2 < 3) {
        p2 = 3;
    }

    if (p1 < 2) {
        p1 = 2;
    }

    genVertices(p1, p2);
}


Sphere::~Sphere()
{
}

// This function takes in parameters 1 and 2 to form a sphere which varies by stacks and slices.
// Functionality currently does not include p1.
void Sphere::genVertices(int p1, int p2) {
    float radius = 0.5;

    // Get how many degrees each angle differs by.
    float theta_seg = (2.f * M_PI) / (p2);
    float psi_seg = M_PI / static_cast<float>(p1);

    makeSide(radius, theta_seg, psi_seg, p1, p2);

    this->setVertexData(m_verts.data(), m_verts.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_verts.size() / 3);
    this->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->buildVAO();
}

// This creates one "side" of the sphere.
// Convert points to Cartesian coordinates and add them to
// the main vector (m_verts). Repeat the 3rd point and 4th points as degenerates.
// Add normals as you go.
void Sphere::makeSide(float radius, float theta_seg, float psi_seg, int p1, int p2) {
      std::vector<float> point1, point2;
      point1.reserve(3);
      point2.reserve(3);

      std::vector<float> normal;
      normal.reserve(3);

    for (int p = 0; p < p1; p++) {

        for (int t = 0; t <= p2; t++) {

            float phi = p * psi_seg;
            float phiPlus = phi + psi_seg;
            float theta = t * theta_seg;

            // Compute cartesian positions and normals.
            point2 = convertCoords(radius, theta, phiPlus);

            m_verts.insert(m_verts.end(), point2.begin(), point2.end());
            normal = makeNormal(point2);
            m_verts.insert(m_verts.end(), normal.begin(), normal.end());

            point1 = convertCoords(radius, theta, phi);

            m_verts.insert(m_verts.end(), point1.begin(), point1.end());
            normal = makeNormal(point1);
            m_verts.insert(m_verts.end(), normal.begin(), normal.end());

        }

    }
}

// Normals for sphere are the coordinates of the vertex multiplied by 2.
std::vector<float> Sphere::makeNormal(std::vector<float> point) {
    return {point[0] * 2.f, point[1] * 2.f, point[2] * 2.f};
}

// This function converts to Cartesian coordinates, giving you a single point in form of a vector.
std::vector<float> Sphere::convertCoords(float radius, float theta, float psi) {
    return {radius * sin(psi) * cos(theta), radius * cos(psi), radius * sin(psi) * sin(theta)};
}
