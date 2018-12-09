#version 400 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates

out vec4 world_position;
out vec4 world_normal;
out vec3 color; // Computed color for this vertex
out vec2 texc;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;
uniform vec2 repeatUV;

//uniform vec3 reflective;
//uniform float blend;
//uniform vec3 attenuation;

//uniform QImage??

uniform bool textureUsed;     // Whether texture is used


void main() {
    texc = texCoord * repeatUV;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    world_position = m * vec4(position, 1.0);
    world_normal = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    gl_Position = p * position_cameraSpace;

    // kept as placeholder bc idk how to compute color rn
    color = ambient_color + diffuse_color;
    color = clamp(color, 0.0, 1.0);
}
