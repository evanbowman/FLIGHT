#version 330 core

out vec4 fragColor;

uniform float intensity;

void main() {
    fragColor = vec4(intensity,
		     intensity,
		     intensity,
		     1.f);
}
