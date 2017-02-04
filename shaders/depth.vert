#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform mat4 model;
uniform mat4 lightSpace;
uniform mat4 invTransModel;

void main() {
    // These parameters are defined but unused so that Sprite::Display()
    // can be repurposed for the depth pass shader
    texCoord;
    normal;
    invTransModel;
    gl_Position = lightSpace * model * vec4(position, 1.0f);
}