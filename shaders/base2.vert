#version 330 core

in vec3 position;
in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;

void main() {
    TexCoord = texcoord;
    gl_Position = proj * view * model * vec4(position.x, position.y, position.z, 1.0);
}