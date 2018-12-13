#version 400 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main(){
    fragColor = texture(tex, uv);
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);

}
