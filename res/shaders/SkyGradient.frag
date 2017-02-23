#version 330 core

out vec4 fragColor;

in vec3 fragPos;

const vec3 skyColorHigh = vec3(0.015, 0.618, 0.94);
const vec3 skyColorLow = vec3(0.3, 0.72, 1.0);

void main() {
    vec3 outColor = skyColorHigh;
    outColor = mix(skyColorLow, skyColorHigh, (fragPos.y -10) / 40);
    fragColor = vec4(outColor, 1.0);
}
