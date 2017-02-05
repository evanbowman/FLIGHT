#version 330 core

in vec3 fragNormal;
in float fragElevation;

const vec3 lightDir = vec3(0.f, 1.f, -0.125f);

out vec4 fragColor;

vec3 getColor(float elevation) {
    const float snowLine = 60.f;
    if (elevation >= snowLine) {
        return vec3(0.973, 0.969, 0.953);
    } else {
        return vec3(0.808, 0.580, 0.533);
    }
}

void main() {
    //vec3 norm = normalize(fragNormal);
    //const float shadowThresh = 0.5;
    //float shading = max(dot(norm, lightDir), 0.0);
    //if (shading < shadowThresh) {
    //    shading = 0.4;
    //} else {
    //    shading = 1.0;
    //}
    vec3 color = getColor(fragElevation);
    fragColor = vec4(1, 1, 1, 1);// vec4(shading * color, 1.f);
}