#version 330 core

in vec3 position;
in vec2 texCoord;

uniform mat4 cameraSpace;
uniform mat4 model;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = texCoord;
    gl_Position = cameraSpace * model * vec4(position, 1.f);
    gl_Position.z = (2 * log(gl_Position.w + 1) / log(10000 + 1) - 1) * gl_Position.w;
}
