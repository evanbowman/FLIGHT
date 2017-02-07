#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;

uniform mat4 model;
uniform mat4 cameraSpace;
uniform mat4 lightSpace;
uniform mat4 invTransModel;

out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragPos;
out vec4 fragPosLightSpace;

void main() {
    fragTexCoord = texCoord;
    fragPos = vec3(model * vec4(position, 1.0f));
    fragNormal = normalize(mat3(invTransModel) * normal);
    gl_Position = cameraSpace * model * vec4(position, 1.0f);
    gl_Position.z = (2 * log(gl_Position.w + 1) / log(10000 + 1) - 1) * gl_Position.w;
    fragPosLightSpace = lightSpace * vec4(fragPos, 1.0f);
}
