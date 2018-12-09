#version 400 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gColor;

in vec4 position;
in vec4 normal;
in vec4 color;
in vec2 texc;

uniform sampler2D tex;
uniform int useTexture = 0;

void main(){
    // kept as placeholder because idk what tex is
    vec3 texColor = texture(tex, texc).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    gColor = vec4(color * texColor, 1);

    gPosition = position;
    gNormal = normal;
}
