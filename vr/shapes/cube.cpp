/**
 * @file   Cube.cpp
 *
 * Implementation of a cube.
 */

#include "Cube.h"
#include "gl/shaders/ShaderAttribLocations.h"

#include <iostream>
#include <string>

Cube::Cube(int p1)
    : Shape()
{
    // If p1 is 0, display a cube with p1 = 1.
    if (p1 == 0) {
        p1 = 1;
    }

    genVertices(p1);
}


Cube::~Cube()
{
}


// This function generates vertices for the six separate faces of the cubes, adds degenerate vertices between them,
// and combines them into one shape.
void Cube::genVertices(int p1) {
    // top face
    makeFace(p1, -0.5, 0.5, -0.5, 0);

    // Add double of last point of last face/starting point for next.
    addVertex(0.5, 0.5, 0.5);
    addNormal(0);
    addVertex(-0.5, 0.5, 0.5);
    addNormal(1);

    // front face
    makeFace(p1, -0.5, 0.5, 0.5, 1);

    // Add double of last point of last face/starting point for next.
    addVertex(0.5, -0.5, 0.5);
    addNormal(1);
    addVertex(0.5, 0.5, 0.5);
    addNormal(2);

    // right face
    makeFace(p1, 0.5, 0.5, 0.5, 2);

    // Add double of last point of last face/starting point for next.
    addVertex(0.5, -0.5, -0.5);
    addNormal(2);
    addVertex(0.5, 0.5, -0.5);
    addNormal(3);

    // back face
    makeFace(p1, 0.5, 0.5, -0.5, 3);

    // Add double of last point of last face/starting point for next.
    addVertex(-0.5, -0.5, -0.5);
    addNormal(3);
    addVertex(-0.5, 0.5, -0.5);
    addNormal(4);

    // left face
    makeFace(p1,-0.5, 0.5, -0.5, 4);

    // bottom face
    makeFace(p1, -0.5, -0.5, 0.5, 5);

    this->setVertexData(m_verts.data(), m_verts.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_verts.size() / 3);
    this->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    this->buildVAO();
}

// This function takes in a variety of parameters to generate the vertices for one side of the face. Behavior
// varies heavily by which face was passed in, in order to store vertices in the correct order.
void Cube::makeFace(int p1, float x, float y, float z, int face) {
        // Values at top of face
        float start_x, start_y, start_z;

        // Each shift should be of 1/p1. Depending on the face, the direction may differ.
        float x_seg = 1.f / static_cast<float>(p1);
        float y_seg = x_seg * -1;
        float z_seg = x_seg;

        // Redefine shifts by what planes a face traverses across.
        if (face == 0 || face == 5) {
            y_seg = 0;
        } else if (face == 1 || face == 3) {
            z_seg = 0;
        } else {
            x_seg = 0;
        }

        // Again, redefine shifts.
        if (face == 2 || face == 5) {
            z_seg = z_seg * -1;
        } else if (face == 3) {
            x_seg = x_seg * -1;
        }

        // Store the start values.
        start_x = x;
        start_y = y;
        start_z = z;

        // There will be p1 * p1 squares on a face.
        for (int row = 0; row < p1; row++) {

             // At each new row, set horizontal value back to initial. Also increment vertical by 1 / p1.
            if (row > 0) {
                if (face == 0 || face == 5) {
                    x = start_x;
                    z += z_seg;
                } else if (face == 1 || face == 3) {
                    x = start_x;
                    y += y_seg;
                } else {
                    z = start_z;
                    y += y_seg;
                }
            }

            for (int col = 0; col <= p1; col++) {
                // Add the first vertex in the column.
                addVertex(x, y, z);
                addNormal(face);

                // Want a degenerate vertex after first vertex in new row.
                if (col == 0 && row > 0) {
                    addVertex(x, y, z);
                    addNormal(face);
                }

                // We also want the bottom vertex, which will be the previous but shifted down a unit.
                if (face == 0 || face == 5) {
                      addVertex(x, y, z + z_seg);
                      addNormal(face);

                    // Add a degenerate vertex if about to start a new row.
                    if (row < p1 - 1 && col == p1) {
                        addVertex(x, y, z + z_seg);
                        addNormal(face);
                    }
                } else {
                    addVertex(x, y + y_seg, z);
                    addNormal(face);

                    if (row < p1 - 1 && col == p1) {
                        addVertex(x, y + y_seg, z);
                        addNormal(face);
                    }
                }

                // Increase horizontal value to get to the next col.
                if (face == 2 || face == 4) {
                    z += z_seg;
                } else {
                    x += x_seg;
                }
            }
        }
    }

// This function adds a vertex to the vector at the specified point.
void Cube::addVertex(float x, float y, float z) {
    m_verts.reserve(3);
    std::vector<float> vertex = {x, y, z};
    m_verts.insert(m_verts.end(), vertex.begin(), vertex.end());
}

// This function adds a normal to the vector based on which face is passed in.
void Cube::addNormal(int face) {
    std::vector<float> normal;

    switch (face) {
        case 0:
            normal = {0.f, 1.f, 0.f};
            break;
        case 1:
            normal = {0.f, 0.f, 1.f};
            break;
        case 2:
            normal = {1.f, 0.f, 0.f};
            break;
        case 3:
            normal = {0.f, 0.f, -1.f};
            break;
        case 4:
            normal = {-1.f, 0.f, 0.f};
            break;
        default:
            normal = {0.f, -1.f, 0.f};
            break;
    }

    m_verts.reserve(3);
    m_verts.insert(m_verts.end(), normal.begin(), normal.end());
}
