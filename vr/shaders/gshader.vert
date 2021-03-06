#version 400 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates

out vec4 world_position;
out vec4 world_normal;
out vec4 ambient; // Ambient color for this vertex
out vec4 specular;
out vec4 diffuse;
out vec2 texc;
out vec4 position_cameraSpace_out;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;
uniform vec2 repeatUV = vec2(1.f);

//uniform vec3 reflective;
//uniform vec3 attenuation

void main() {
    texc = texCoord * repeatUV;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    position_cameraSpace_out = position_cameraSpace;
    position_cameraSpace_out.x += 5.f;
    position_cameraSpace_out.y += 5.f;
    position_cameraSpace_out.z += 5.f;

    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    world_position = m * vec4(position, 1.0);
    world_normal = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    world_normal = normal_cameraSpace;

    gl_Position = p * position_cameraSpace;

    ambient = vec4(clamp(ambient_color, 0.0, 1.0), 0.f);
    ambient.a = shininess;

    diffuse = vec4(diffuse_color, 0.f);
    diffuse = clamp(diffuse, 0.f, 1.f);

    specular = vec4(clamp(specular_color, 0.0, 1.0), 0.f);
}
