#version 330 core

uniform sampler2D tex;
uniform vec3 eyePos;

struct Material {
    float diffuse;
    float specular;
    float shininess;
};

uniform Material material;

out float fragDepth;

void main() {
    material.diffuse;
    material.specular;
    material.shininess;
    eyePos;
    tex;
    fragDepth = gl_FragCoord.z;
}
