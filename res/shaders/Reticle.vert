#version 330 core

in vec3 position;
in vec2 texCoord;

uniform mat4 proj;
uniform mat4 model;

out vec3 fragPos;
out vec2 fragTexCoord;

void main() {
    fragPos = vec3(model * vec4(position, 1.0f));
    fragTexCoord = texCoord;
    gl_Position = proj * model * vec4(position, 1.f);
}
