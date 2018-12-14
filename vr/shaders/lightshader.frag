#version 400 core

in vec2 texc;

out vec4 fragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gAmbient;
uniform sampler2D gSpecular;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightColors[MAX_LIGHTS];
uniform vec3 v;

void main(){    
    vec4 fragPosition = texture(gPosition, texc);
    vec4 fragNormal = texture(gNormal, texc);

    vec3 fragAmbient = texture(gAmbient, texc).rgb;
    float shininess = texture(gAmbient, texc).a;

    vec4 diffuse_color = texture(gDiffuse, texc);
    vec4 spec = texture(gSpecular, texc);

    vec4 v4 = vec4(v, 1.f);

    // What range are we in?
    vec4 color = vec4(fragAmbient, 1.0f);

//    for (int i = 0; i < MAX_LIGHTS; i++) {
//        vec4 vertexToLight = vec4(0);
//        // Point Light
//        if (lightTypes[i] == 0) {
//            vertexToLight = normalize(v4 * vec4(lightPositions[i], 1) - fragPosition);
//        } else if (lightTypes[i] == 1) {
//            // Dir Light
//            vertexToLight = normalize(v4 * vec4(-lightDirections[i], 0));
//        }

//        float nm = clamp(dot(fragNormal, vertexToLight), 0.f, 1.f);
//        vec4 r = normalize(2.f * fragNormal * nm - vertexToLight);

//        // v should be -d, or view vector.
//        float rv = pow(clamp(dot(r, v4), 0.f, 1.f), shininess);

//        // Add diffuse component; need normal in camera space :/
//        diffuse_color = diffuse_color * nm + spec * rv;

//        color += vec4(lightColors[i], 1) * diffuse_color;

//        // REMEMBER ATTENUATION

//        // Add specular component - kept to help find -d (AKA v)
//        //vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
//        //vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
//        //float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
//    }

    fragColor = color;
}
