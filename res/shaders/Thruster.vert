#version 330 core

uniform mat4 cameraSpace;
uniform mat4 model;

in vec3 position;

out vec3 fragPos;

void main() {
    fragPos = vec3(model * vec4(position, 1.0f));
    gl_Position = cameraSpace * model * vec4(position, 1.f);
    gl_Position.z = (2 * log(gl_Position.w + 1) / log(1000 + 1) - 1) * gl_Position.w;
}
