#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec3 eyePos;

const vec3 lightDir = vec3(0.f, 1.f, -1.125f);

out vec4 fragColor;

vec3 getColor(float elevation) {
    const float snowLine = 2.f;
    if (elevation >= snowLine) {
        return vec3(0.973, 0.969, 0.953);
    } else {
        return vec3(0.658, 0.530, 0.503);
    }
}

void main() {
    vec3 norm = normalize(fragNormal);
    const float shadowThresh = 0.5;
    float shading = max(dot(norm, lightDir), 0.0);
    float epsilon = fwidth(shading);
    if (shading > shadowThresh - epsilon && shading < shadowThresh + epsilon) {
        shading = mix(0.5, 1.0, smoothstep(shadowThresh - epsilon, shadowThresh + epsilon, shading));
    } else if (shading < shadowThresh) {
        shading = 0.5;
    } else {
        shading = 1.0;
    }
    vec3 color = getColor(fragPos.y);
    fragColor = vec4(shading * color, 1.f);
}