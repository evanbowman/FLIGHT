#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
uniform vec3 eyePos;

const vec3 lightDir = vec3(0.f, 1.f, -1.125f);

out vec4 fragColor;

vec3 getColor(float elevation) {
    const float snowLine = 3.f;
    if (elevation >= snowLine) {
        return vec3(0.973, 0.969, 0.953);
    } else {
        return vec3(0.595, 0.520, 0.495);
    }
}

vec3 desaturate(vec3 color, float amount) {
    vec3 gray = vec3(dot(vec3(0.299, 0.587, 0.114), color));
    return mix(color, gray, amount);
}

vec3 atmPerspective(vec3 color, float amount) {
    const vec3 skyColor = vec3(0.f, 0.42f, 0.70f);
    color = desaturate(color, amount);
    return mix(color, skyColor, amount);
}

void main() {
    vec3 norm = normalize(fragNormal);
    const float shadowThresh = 0.5;
    float light = max(dot(norm, lightDir), 0.0);
    if (light < shadowThresh) {
        light = 0.6;
    } else {
        light = 1.0;
    }
    vec3 color = getColor(fragPos.y);
    if (light < 1.0) {
	color.z = max(color.z, color.z + 0.05);
    }
    color = light * color;
    float scaledDist = min(1.f, length(fragPos - eyePos) / 375);
    color = atmPerspective(color, scaledDist);
    fragColor = vec4(color, 1.f);
}
