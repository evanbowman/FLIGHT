#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragVert;

void main() {
    fragTexCoord = texCoord;
    fragNormal = normal;
    fragVert = position;
    // FIXME: it's wasteful to do 'proj * view' for every vertex
    gl_Position = proj * view * model * vec4(position.x, position.y, position.z, 1.0);
}