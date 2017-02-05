#version 330 core

uniform sampler2D tex;
uniform sampler2D shadowMap;

uniform vec3 eyePos;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;
in vec4 fragPosLightSpace;

struct Material {
    float diffuse;
    float specular;
    float shininess;
};

uniform Material material;

out vec4 fragColor;

const vec3 lightDir = vec3(0.f, 1.f, -0.125f);

float calcShadow(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;
    return shadow;
}  

void main() {
    vec3 norm = normalize(fragNormal);
    const float bin1 = 0.5;
    const float bin2 = 1.0;
    float diff = max(dot(norm, lightDir), 0.0);
    float epsilon = fwidth(diff);
    if (diff > bin1 - epsilon && diff < bin1 + epsilon) {
        diff = mix(0.4, bin2, smoothstep(bin1 - epsilon, bin1 + epsilon, diff));
    } else if (diff < bin1) {
        diff = 0.4;
    } else {
        diff = bin2;
    }

    vec3 eyeDir = normalize(eyePos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(eyeDir, reflectDir), 0.0), material.shininess);
    epsilon = fwidth(spec);
    if (spec > 0.5 - epsilon && spec < 0.5 + epsilon) {
        spec = smoothstep(0.5 - epsilon, 0.5 + epsilon, spec);
    } else {
        spec = step(0.5, spec);
    }
    float shadow = calcShadow(fragPosLightSpace);
    if (shadow > 0) {
        diff = 0.2;
	spec = 0.2;
    }
    fragColor = ((spec * material.specular + diff * material.diffuse) + 0.5) * texture(tex, fragTexCoord);
}
