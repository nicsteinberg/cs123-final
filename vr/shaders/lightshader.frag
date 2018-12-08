#version 400 core

in vec2 texc;

out vec4 fragColor;

uniform sampler2D gColor;
uniform sampler2D gNormal;
uniform sampler2D gPosition;

// Until we figure out something better
uniform sampler2D gDiffuse;
uniform sampler2D gTexture;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightColors[MAX_LIGHTS];
uniform vec3 v;

uniform int useTexture = 0;

void main(){    
    vec4 fragPosition = texture(gPosition, texc);
    vec4 fragNormal = texture(gNormal, texc);
    vec3 fragRGB = texture(gColor, texc).rgb;
    float spec = texture(gColor, texc).a;

    vec3 diffuse_color = texture(gDiffuse, texc).rgb;
    float shininess = texture(gDiffuse, texc).a;

    vec4 tex = texture(gTexture, texc).rgb;
    float blend = texture(gTexture, texc).a;

    // What range are we in?
    vec4 color = vec4(fragRGB, 1.0f);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        vec4 vertexToLight = vec4(0);
        // Point Light
        if (lightTypes[i] == 0) {
            vertexToLight = normalize(v * vec4(lightPositions[i], 1) - fragPosition);
        } else if (lightTypes[i] == 1) {
            // Dir Light
            vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
        }

        float nm = clamp(dot(fragNormal, vertexToLight), 0.f, 1.f);
        vec4 r = normalize(2.f * fragNormal * nm - vertexToLight);

        // v should be -d, or view vector.
        float rv = pow(clamp(dot(r, v), 0.f, 1.f), shininess);

        // Add diffuse component; need normal in camera space :/
        diffuse_color = (diffuse_color * (1.f - blend) + tex * blend) * nm + spec * rv;

        color += lightColors[i] * diffuse_color;

        // REMEMBER ATTENUATION

        // Add specular component - kept to help find -d (AKA v)
        //vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
        //vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
        //float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
    }
}
