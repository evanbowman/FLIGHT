#version 330 core

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 fragColor;

const float baseAlpha = 0.15;

float l;

float RadialGradient(float rad) {
    if (l < rad) {
	return 1.f;
    }
    return 0.f;
}

void main() {
    l = length(fragTexCoord - vec2(0.5, 0.5));
    float r1 = baseAlpha * RadialGradient(0.425);
    fragColor = vec4(0, 0, 0.1, r1);
}
