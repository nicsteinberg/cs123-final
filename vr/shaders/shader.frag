#version 400 core

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 camera_pos;

in vec3 color;
in vec2 texc;
in vec4 position_cameraSpace_out;

uniform sampler2D tex;
uniform int useTexture = 0;

void main(){
    vec3 texColor = texture(tex, texc).rgb;
    texColor.r = texture(tex, texc).b;
    texColor.b = texture(tex, texc).r;

    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    outColor = vec4(color * texColor, 1);

//        if (useTexture == 1) {
//        fragColor = vec4(texColor, 1.f);
//    }

    camera_pos = position_cameraSpace_out;
}
