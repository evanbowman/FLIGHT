#version 330 core

in vec3 position;

uniform mat4 model;
uniform mat4 cameraSpace;

void main() {
    gl_Position = cameraSpace * model * vec4(position, 1.0f);
    gl_Position.z = (2 * log(gl_Position.w + 1) / log(10000 + 1) - 1) * gl_Position.w;
}
