#version 400 core

layout (location = 0) out vec4 fragColor;
//layout (location = 1) out vec4 camera_pos;

in vec3 color;
in vec2 texc;
//in vec4 position_cameraSpace_out;

//out vec4 fragColor;
//out vec4 camera_pos;

//out vec4 textures[2];

uniform sampler2D tex;
uniform int useTexture = 0;

void main(){
    vec3 texColor = texture(tex, texc).rgb;
    //fragColor = vec4(texture(tex, vec2(1, 0)).rgb, 1);
    //fragColor = vec4(1.f);
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    fragColor = vec4(color * texColor, 1);

//    textures[0] = vec4(color * texColor, 1);
//    textures[1] = position_cameraSpace_out;

//    // This never happens, so at this point, they differ correctly.
//    if (vec4(color * texColor, 1) == position_cameraSpace_out) {
//        textures[0] = vec4(1.f);
//        textures[1] = vec4(1.f);
//        //textures[1] = position_cameraSpace_out;
//    }

    //fragColor = texColor;
    //camera_pos = position_cameraSpace_out;
}
