#version 330 core

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 fragColor;

float l;
float epsilon;

float Circle(float rad) {
    return smoothstep(rad, rad - epsilon, l);
}

void main() {
    l = length(fragTexCoord - vec2(0.5, 0.5));
    epsilon = fwidth(l);
    fragColor = vec4(vec3(0, 0, 0.1), 0.5 * Circle(0.5));
}
