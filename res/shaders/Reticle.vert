#version 330 core

in vec3 position;

uniform mat4 proj;
uniform mat4 model;

out vec3 fragPos;

void main() {
    fragPos = vec3(model * vec4(position, 1.0f));
    gl_Position = proj * model * vec4(position, 1.f);
}
