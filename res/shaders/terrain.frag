#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
uniform vec3 eyePos;

out vec4 fragColor;

const float shadowThresh = 0.5;
const vec3 lightDir = vec3(0.f, 1.f, -1.f);
const vec3 skyColor = vec3(0.2f, 0.62f, 0.90f);
const vec3[2] terrainColors = vec3[2](vec3(0.973, 0.969, 0.953),
				      vec3(0.595, 0.520, 0.495));

int getColorId(float elevation) {
    const float snowLine = 3.f;
    return int(elevation < snowLine);
}

void desaturate(inout vec3 color, float amount) {
    vec3 gray = vec3(dot(vec3(0.299, 0.587, 0.114), color));
    color = mix(color, gray, amount);
}

void atmPerspective(inout vec3 color, float amount) {
    color = desaturate(color, amount);
    color = mix(color, skyColor, amount);
}

void main() {
    vec3 norm = normalize(fragNormal);
    float light = max(dot(norm, lightDir), 0.0);
    if (light < shadowThresh) {
        light = 0.6;
    } else {
        light = 1.0;
    }
    vec3 color = terrainColors[getColorId(fragPos.y)];
    if (light < 1.0) {
	color.z = max(color.z, color.z + 0.05);
    }
    color = light * color;
    float scaledDist = min(1.f, length(fragPos - eyePos) / 375);
    atmPerspective(color, scaledDist);
    fragColor = vec4(color, 1.f);
}
