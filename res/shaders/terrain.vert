#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform mat4 model;
uniform mat4 cameraSpace;
uniform mat4 invTransModel;

out vec3 fragNormal;
out float fragElevation;

void main() {
    texCoord;
    fragElevation = vec3(model * vec4(position, 1.0f)).y;
    fragNormal = normalize(mat3(invTransModel) * normal);
    gl_Position = cameraSpace * model * vec4(position, 1.0f);
}
