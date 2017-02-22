#version 330 core

uniform mat4 proj;
uniform mat4 model;

layout (location = 0) in vec4 vertex;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = vertex.zw;
    gl_Position = proj * model * vec4(vertex.xy, 0.f, 1.f);
}
