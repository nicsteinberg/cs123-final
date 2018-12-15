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
     float focus = 0;
     vec4 position = texture(camera_pos_tex, uv);
     float distance = sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
//     float x = max(0, min(20, abs(distance - focus))) + 1.f;
//     int supportWidth = max(0, int (6.39 * log(1.14 * x)) + 1);
//     supportWidth = int (distance);
//     if (distance > focus) {
         float x = distance - focus;
//         supportWidth = int (8.25 * log(1.13 * x)) + 1;
//     }
         if (x < 1) {
             supportWidth = 1;
         } else if (x < 2) {
             supportWidth = 3;
         } else if (x < 3) {
             supportWidth = 10;
         } else {
             supportWidth = 20;
         }

//    int supportWidth = int(texture(camera_pos_tex, uv).x * 20.f);
//    int supportWidth = 1;
//    const int supportWidth = 20;

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

//    outColor = vec4(1.f);
//    outColor = vec4(texture(camera_pos_tex, uv));
    outColor = vec4(distance + 0.f);
}
