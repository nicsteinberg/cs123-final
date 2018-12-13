#version 400 core

layout(location = 0) in vec3 position;
layout(location = 5) in vec2 inUV;

out vec2 uv;
in vec4 camera_pos_in;
out vec4 camera_pos_out;

void main() {
    uv = inUV;
    gl_Position = vec4(position, 1.0);
    camera_pos_out = camera_pos_in;
}
