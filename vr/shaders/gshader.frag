#version 400 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gAmbient;
layout (location = 4) out vec4 gSpecular;
layout (location = 5) out vec4 gCameraPos;

in vec4 world_position;
in vec4 world_normal;
in vec4 ambient;
in vec4 specular;
in vec4 diffuse;
in vec2 texc;
in vec4 position_cameraSpace_out;

// May vary over texture, if set to texture
uniform sampler2D tex;

uniform int useTexture = 0;

void main(){
    gPosition = world_position;
    gNormal = world_normal;
    gAmbient = ambient;
    gSpecular = specular;

    if (useTexture == 1) {
        gDiffuse = texture(tex, texc);
    } else {
        gDiffuse = diffuse;
    }

    gCameraPos = position_cameraSpace_out;
}
