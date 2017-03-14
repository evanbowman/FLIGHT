#version 330 core

uniform mat4 proj;
uniform mat4 model;

in vec3 position;
in vec2 texCoord;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = texCoord;
    gl_Position = proj * model * vec4(position, 1.f);
}
