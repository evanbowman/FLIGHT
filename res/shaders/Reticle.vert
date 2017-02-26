#version 330 core

in vec3 position;
in vec2 texCoord;

uniform mat4 proj;
uniform mat4 model;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = texCoord;
    gl_Position = proj * model * vec4(position, 1.f);
}
