#version 330 core

uniform sampler2D tex;
uniform vec3 eyePos;

out float fragDepth;

void main() {
    eyePos;
    tex;
    fragDepth = gl_FragCoord.z;
}
