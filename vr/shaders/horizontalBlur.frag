#version 400 core

in vec2 uv;

uniform sampler2D tex;
//uniform sampler2D camera_pos_tex;

//uniform sampler2D textures[2];

out vec4 fragColor;

void main(){
    fragColor = vec4(1.0);

    // TODO [Task 9] Calculate the texelSize
    vec2 texelSize = 1.0 / textureSize(tex, 0).xy;
//    vec2 texelSize = 1.0 / textureSize(textures[0], 0).xy;

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
//        fragColor += weight * texture(textures[0], uv + vec2(i * texelSize.x, 0));
        weights += weight;
    }
    fragColor /= weights;

    //fragColor = vec4(texture(camera_pos_tex, uv));
    //1 and 0 indices give you same thing
        //fragColor = vec4(texture(textures[0], uv));

//        if (vec4(texture(tex, uv)) == vec4(texture(camera_pos_tex, uv))) {
//            fragColor = vec4(1.f);
//        }
}
