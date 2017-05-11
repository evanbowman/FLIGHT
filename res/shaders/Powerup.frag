#version 330 core

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 fragColor;

uniform float fill;

float l;
float epsilon;

const vec3 fgColor = vec3(0.949, 0.765, 0.027);
const vec3 bgColor = vec3(0, 0, 0.1);
const float iconOpacity = 0.5;

float Circle(float rad) {
    return smoothstep(rad, rad - epsilon, l);
}

void main() {
    l = length(fragTexCoord - vec2(0.5, 0.5));
    epsilon = fwidth(l);
    float bgCirc = Circle(0.5);
    float fgCirc = Circle(0.42);
    vec3 color = bgColor;
    if (fgCirc > 0.f) {
        if (fragTexCoord.y < fill) {
            color = mix(bgColor, fgColor, fgCirc);
        }
    }
    fragColor = vec4(color, iconOpacity * bgCirc);
}
