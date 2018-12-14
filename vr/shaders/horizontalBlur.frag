#version 400 core

in vec2 uv;

uniform sampler2D tex;
//uniform sampler2D camera_pos_tex;

out vec4 fragColor;

void main(){
    fragColor = vec4(1.0);

    // TODO [Task 9] Calculate the texelSize
    vec2 texelSize = 1.0 / textureSize(tex, 0).xy;

    // float focus = 5;
    // float position = texture(camera_pos_tex, uv);
    // float distance = sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
    // float depth = max(0, min(25, distance));
    // int x = depth;
    // if (depth < focus) {
    //      x = 2 * focus - depth;
    // int supportWidth = 11.3 * log(0.237 * x);

//    int supportWidth = int(texture(camera_pos_tex, uv).x * 20.f);
    const int supportWidth = 20;

    fragColor = vec4(0.0);
    float weights = 0.0;
    for (int i = -supportWidth; i <= supportWidth; i++) {
        float weight = (supportWidth + 1) - abs(i);
        // TODO [Task 9] Add weight * sampleColor to fragColor, where
        //               sampleColor = tex sampled at uv + the offset
        //               in the x direction (you are moving over by "i" texels)
        fragColor += weight * texture(tex, uv + vec2(i * texelSize.x, 0));
        weights += weight;
    }
    fragColor /= weights;

//    fragColor = vec4(texture(camera_pos_tex, uv));
}
