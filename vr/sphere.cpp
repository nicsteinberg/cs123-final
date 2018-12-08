#include "sphere.h"
#include <math.h>
#include <algorithm>
#include <iostream>

// Class to implement the vertex data of a sphere.
Sphere::Sphere(int shapeParameter1, int shapeParameter2)
    : Shape(shapeParameter1, shapeParameter2)
{
    m_parameter1 = std::max(2, m_parameter1);
    m_parameter2 = std::max(3, m_parameter2);
}

Sphere::~Sphere() {

}

// Computes the cartesian coordinates on a sphere given radius and latitudinal and longitudinal angles.
std::vector<float> Sphere::coordinatesOnSphere(float radius, float theta, float phi) {
    return {radius * sin(phi) * cos(theta), radius * cos(phi), radius * sin(phi) * sin(theta)};
}

// Returns, a vector<float> with each value doubled. Using this function because the cartesian coordinates of a position
// on a sphere about the origin are co-directional to that point's normal, but magnitude = radius. When radius = 0.5,
// doubling position coordinates gives the normalized normal vector.
std::vector<float> Sphere::doubleEachValue(std::vector<float> input) {
    std::vector<float> output;
    output.reserve(input.size());
    for (unsigned int i = 0; i < input.size(); i++) {
        output.push_back(input[i] * 1);
    }
    return output;
}

std::vector<float> normalize(std::vector<float> input) {
    // todo
    float current_magnitude = 0;
    for (unsigned int i = 0; i < input.size(); i++) {
        current_magnitude += input[i] * input[i];
    }
    current_magnitude = sqrt(current_magnitude);

    std::vector<float> normalized;
    normalized.resize(input.size());

    for (unsigned int i = 0; i < normalized.size(); i++) {
        normalized[i] = input[i] / current_magnitude;
    }

    return normalized;
}

// Computes the vertices of a sphere based on tesselation parameters.
std::vector<float> Sphere::makeFace() {

    // We'll store the output here.
    std::vector<float> vertices, point1, point2, normal1, normal2;
    point1.reserve(3);
    point2.reserve(3);
    normal1.reserve(3);
    normal2.reserve(3);

    float thetaIncrement = 2 * M_PI / m_parameter2;
    float phiIncrement = M_PI / m_parameter1;
    float rad = 0.5;

    for (int p = 0; p < m_parameter1; p++) {

        for (int t = 0; t <= m_parameter2; t++) {

            vertices.reserve(4);

            float phi = p * phiIncrement;
            float phiPlus = phi + phiIncrement;
            float theta = t * thetaIncrement;

            // Compute cartesian positions and normals.

            point2 = coordinatesOnSphere(rad, theta, phiPlus);
            vertices = append(vertices, point2);
            vertices = append(vertices, normalize(point2));

            point1 = coordinatesOnSphere(rad, theta, phi);
            vertices = append(vertices, point1);
            vertices = append(vertices, normalize(point1));

        }

    }


    return vertices;
}

// Sets up the vertices of a sphere.
std::vector<float> Sphere::makeVertices() {

    std::vector<float> vertexStrip = {};
    vertexStrip = append(vertexStrip, makeFace());

    return vertexStrip;
}
