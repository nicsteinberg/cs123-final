#version 400 core

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 camera_pos;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gAmbient;
uniform sampler2D gSpecular;
uniform sampler2D gCameraPos;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightAttenuations[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform mat4 v;

void main(){    
    vec4 fragPosition = texture(gPosition, uv);
    vec4 fragNormal = texture(gNormal, uv);

    vec3 fragAmbient = texture(gAmbient, uv).rgb;
    float shininess = texture(gAmbient, uv).a;

    vec4 diffuse_color = texture(gDiffuse, uv);
    vec4 spec = texture(gSpecular, uv);

    camera_pos = texture(gCameraPos, uv);

    vec4 color = vec4(fragAmbient, 1.0f);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        vec4 vertexToLight = vec4(0);
        // Point Light
        if (lightTypes[i] == 0) {
            vertexToLight = normalize(v * vec4(lightPositions[i], 1) - camera_pos + 5.f);
        } else if (lightTypes[i] == 1) {
            // Dir Light
            vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
        }

//        float nm = clamp(dot(fragNormal, vertexToLight), 0.f, 1.f);
//        vec4 r = normalize(2.f * fragNormal * nm - vertexToLight);

//        // v should be -d, or view vector.
//        vec4 eyeDirection = normalize(vec4(0,0,0,1) - camera_pos + 5.f);
//        float rv = pow(clamp(dot(r, -eyeDirection), 0.f, 1.f), shininess);

//        //color = vec4(rv);

//        // Add diffuse component.
//        vec4 diffuse = diffuse_color * nm;// + spec * rv;
//        diffuse = spec * rv;

//        color += vec4(lightColors[i], 1) * diffuse;

        // ATTENUATION
        float dist = sqrt(vertexToLight.x * vertexToLight.x + vertexToLight.y * vertexToLight.y + vertexToLight.z * vertexToLight.z);
        float att = min(1 / (lightAttenuations[i].x + lightAttenuations[i].y * dist + lightAttenuations[i].z * dist * dist), 1.f);

        float diffuseIntensity = max(0.0, dot(vertexToLight, fragNormal));
        color += max(vec4(0), vec4(lightColors[i], 0) * diffuse_color * diffuseIntensity * att);

        vec4 lightReflection = normalize(-reflect(vertexToLight, fragNormal));
        vec4 eyeDirection = normalize(vec4(0,0,0,1) - camera_pos + 5.f);
        float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
        color += max(vec4(0), vec4(lightColors[i], 0) * spec * specIntensity * att);

        // To get rid of error.
        color += vec4(0.0000000000001f) * fragPosition;
    }

    color = clamp(color, 0.f, 1.f);
    outColor = color;
}
