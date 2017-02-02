#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 invTransModel;

out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragPos;

void main() {
    fragTexCoord = texCoord;
    fragPos = vec3(model * vec4(position, 1.0f));
    fragNormal = normalize(mat3(invTransModel) * normal);
    gl_Position = proj * view * model * vec4(position, 1.0f);
}