#version 330 core

in vec3 position;

uniform mat4 VP;
uniform mat4 model;

void main() {
    gl_Position = VP * model * vec4(position, 1);
}
