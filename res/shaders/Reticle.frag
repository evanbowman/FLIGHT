#version 330 core

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 fragColor;

const float baseAlpha = 0.5;

float l;
float epsilon;

float Circle(float rad) {
    return smoothstep(rad, rad - epsilon, l);
}

void main() {
    l = length(fragTexCoord - vec2(0.5, 0.5));
    epsilon = fwidth(l);
    float c1 = baseAlpha * Circle(0.5) - baseAlpha * Circle(0.425);
    float c2 = baseAlpha * Circle(0.1) - baseAlpha * Circle(0.05);
    fragColor = vec4(1, 1, 1, c1 + c2);
}
