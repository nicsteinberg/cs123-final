/**
 * @file   Cylinder.cpp
 *
 * Implementation of a cylinder.
 */

#include "cylinder.h"
#include "Cap.h"
#include "gl/shaders/ShaderAttribLocations.h"

#include <iostream>
#include <cmath>
#include <math.h>

Cylinder::Cylinder(int p1, int p2)
    : Shape()
{
    genVertices(p1, p2);
}


Cylinder::~Cylinder()
{
}

// This function takes in parameters 1 and 2 to form a cylinder which varies by stacks and slices.
// Functionality does not include p1.
void Cylinder::genVertices(int p1, int p2) {
    // Create the caps for the cylinder.
    m_cap = std::make_unique<Cap>(p1, p2, 0.5);
    m_cap2 = std::make_unique<Cap>(p1, -p2, -0.5);

    // Now begin the barrel of the cylinder.
    float radius = 0.5;
    float y = 0.5;
    std::vector<float> pt1, pt2, pt3, pt4;

    // There must be at least three "sides" to the cylinder.
    if (p2 < 3) {
        p2 = 3;
    }

    // Get how many degrees each angle differs by.
    float theta_seg = (2.f * M_PI) / static_cast<float>(p2);
    float theta = 0;

    // For each "side" of the cylinder, draw triangle strips similar to their counterparts
    // on the cylinder caps. Convert points to Cartesian coordinates and add them to
    // the main vector (m_verts).
    for (int i = 0; i < p2; i++) {
        makeRectangle(radius, y, theta, theta_seg, p1);

        theta += theta_seg;
    }

    this->setVertexData(m_verts.data(), m_verts.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_verts.size() / 3);
    this->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->buildVAO();
}

// This overrides openglshape's draw so it will draw both the barrel and the caps.
void Cylinder::draw() {
    m_cap->draw();
    m_cap2->draw();
    this->OpenGLShape::draw();
}

// This function converts to Cartesian coordinates, giving you a single point in form of a vector.
std::vector<float> Cylinder::convertCoords(float radius, float y, float theta) {
    return {radius * cos(theta), y, radius * sin(theta)};
}

// Convert points to Cartesian coordinates and add them to
// the main vector (m_verts). Repeat the 3rd point as a degenerate.
// Add normals as you go.
void Cylinder::makeRectangle(float radius, float y, float theta, float theta_seg, int p1) {
    std::vector<float> pt1, pt2, pt3, pt4;
    std::vector<float> normal;

    pt1 = convertCoords(radius, y, theta + theta_seg);
    normal = makeNormal(theta + theta_seg);
    m_verts.insert(m_verts.end(), pt1.begin(), pt1.end());
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());
    getUV(pt1);

    pt2 = convertCoords(radius, -y, theta + theta_seg);
    m_verts.insert(m_verts.end(), pt2.begin(), pt2.end());
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());
    getUV(pt2);

    pt3 = convertCoords(radius, y, theta);
    normal = makeNormal(theta);
    m_verts.insert(m_verts.end(), pt3.begin(), pt3.end());
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());
    getUV(pt3);

    pt4 = convertCoords(radius, -y, theta);
    m_verts.insert(m_verts.end(), pt4.begin(), pt4.end());
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());
    getUV(pt4);
}

std::vector<float> Cylinder::makeNormal(float theta) {
    return {cos(theta), 0.f, sin(theta)};
}

void Cylinder::getUV(std::vector<float> intersection) {
    std::vector<float> uv = {0, 0};

    // Caps return plane uv coordinates.
    if (intersection[1] < 0.5f + 1e-4 && intersection[1] > 0.5f - 1e-4) {
        uv = {intersection[0] + 0.5f, 1.f - -intersection[2] + 0.5f};

    } else if (intersection[1] < -0.5f + 1e-4 && intersection[1] > -0.5f - 1e-4) {

        uv = {intersection[0] + 0.5f, 1.f - intersection[2] + 0.5f};

    } else {

        // If on the curved surface, use position around perimeter to determine u.
        float theta = atan2(intersection[2], intersection[0]);

        uv[0] = -theta / (2 * M_PI);

        if (theta >= 0.f) {
            uv[0] += 1.f;
        }

        uv[1] = 1.f - intersection[2] + 0.5f;
    }

    m_verts.insert(m_verts.end(), uv.begin(), uv.end());
}

