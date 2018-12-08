/**
 * @file   Cap.cpp
 *
 * Implementation of a shared base between a cone and a cylinder.
 */

#include "Cap.h"
#include "gl/shaders/ShaderAttribLocations.h"

#include <cmath>
#include <math.h>
#include <iostream>

Cap::Cap(int p1, int p2, float y)
    : Shape()
{
    genVertices(p1, p2, y);
}


Cap::~Cap()
{
}

// This function takes in parameters 1 and 2 to form a cap which serves as a base to both a cylinder and a cone.
// As p1 and p2 increase, so do the cap's "stacks and slices". It also takes in a y value to determine if it is
// the top of the cylinder or the bottom.
void Cap::genVertices(int p1, int p2, float y) {
    float radius = 0.5;
    std::vector<float> pt1, pt2, pt3;

    // There must be at least three triangles on the cap.
    if (p2 < 3 && p2 >= 0) {
        p2 = 3;
    } else if (p2 > -3 && p2 < 0) {
        p2 = -3;
    }

    // Get how many degrees each angle differs by.
    float theta_seg = (2.f * M_PI) / static_cast<float>(p2);
    float theta = 0.f;

    // Reserve the size of 12 (4 vertices) times the number of times we go through the for loop. Also multiply by 2 for normals.
    m_verts.reserve(2 * 12 * abs(p2));

    // Add normals depending on if the cap is at the top or bottom of the shape.
    std::vector<float> normal = {0.f, y / 0.5f, 0.f};

    // Draw p2 triangle strips.
    for (int i = 0; i < abs(p2); i++) {
        makeTriangle(radius, y, theta, theta_seg, normal, p1);

        theta -= theta_seg;
    }

    this->setVertexData(m_verts.data(), m_verts.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_verts.size() / 3);
    this->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->buildVAO();
}

// This function converts to Cartesian coordinates, giving you a single point in form of a vector.
std::vector<float> Cap::convertCoords(float radius, float y, float theta) {
    return {radius * cos(theta), y, radius * sin(theta)};
}

// Convert points to Cartesian coordinates and add them to
// the main vector (m_verts). Repeat the 3rd point as a degenerate.
// Add normals as you go.
void Cap::makeTriangle(float radius, float y, float theta, float theta_seg, std::vector<float> &normal, int p1) {
    std::vector<float> pt1, pt2, pt3;

    // Zigzag across the triangle - the number of times and size of each subtriangle will depend on p1.
    // When we get to the center of the circle, we'll repeat it as a degenerate.
    for (int i = 0; i <= p1; i++) {
        pt1 = convertCoords((p1 - i) * radius / p1, y, theta);
        pt2 = convertCoords((p1 - i) * radius / p1, y, theta - theta_seg);

        m_verts.insert(m_verts.end(), pt1.begin(), pt1.end());
        m_verts.insert(m_verts.end(), normal.begin(), normal.end());

        m_verts.insert(m_verts.end(), pt2.begin(), pt2.end());
        m_verts.insert(m_verts.end(), normal.begin(), normal.end());
    }

    // Repeat the last point as a degenerate.
    pt3 = convertCoords(radius, y, theta - theta_seg);

    m_verts.insert(m_verts.end(), pt3.begin(), pt3.end());
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());

    m_verts.insert(m_verts.end(), pt3.begin(), pt3.end());
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());
}
