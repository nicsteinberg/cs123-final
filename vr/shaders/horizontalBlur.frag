#version 400 core

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 camera_pos;

in vec2 uv;

uniform sampler2D tex;
uniform sampler2D camera_pos_tex;

//out vec4 fragColor;

void main(){
    outColor = vec4(1.0);

    // TODO [Task 9] Calculate the texelSize
    vec2 texelSize = 1.0 / textureSize(tex, 0).xy;

    int supportWidth = 1;
    float focus = 6.f;
    vec4 position = texture(camera_pos_tex, uv);
    float pos_x = position.x - 5.f;
    float pos_y = position.y - 5.f;
    float pos_z = position.z - 5.f;
    float distance = sqrt(pos_x * pos_x + pos_y * pos_y + pos_z * pos_z);
    float x = distance - focus;

    if (distance < 5.5) {
        supportWidth = 30;
    } else if (distance < 5.7) {
        supportWidth = 25;
    } else if (distance < 6.f) {
        supportWidth = 20;
    } else if (distance < 6.299) {
        supportWidth = 10;
    } else if (distance < 6.5) {
        supportWidth = 4;
    } else if (distance < 6.9) {
        supportWidth = 2;
    } else if (distance < 7.5) {
        supportWidth = 1;
    } else if (distance < 7.545) {
        supportWidth = 12;
    } else if (distance < 7.6) {
        supportWidth = 25;
    } else if (distance < 7.7) {
        supportWidth = 30;
    } else {
        supportWidth = 30;
    }

    outColor = vec4(0.0);
    float weights = 0.0;
    for (int i = -supportWidth; i <= supportWidth; i++) {
        float weight = (supportWidth + 1) - abs(i);
        // TODO [Task 9] Add weight * sampleColor to fragColor, where
        //               sampleColor = tex sampled at uv + the offset
        //               in the x direction (you are moving over by "i" texels)
        outColor += weight * texture(tex, uv + vec2(i * texelSize.x, 0));
        weights += weight;
    }
    outColor /= weights;
    camera_pos = texture(camera_pos_tex, uv);


}
