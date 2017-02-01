#version 330 core

uniform sampler2D tex;
uniform vec3 eyePos;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(fragNormal);
    // we're outdoors, light comes from the sky
    vec3 lightDir = vec3(0.25f, 1.f, 0.f);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    float specularStrength = 0.5f;
    vec3 eyeDir = normalize(eyePos - fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = pow(max(dot(eyeDir, reflectDir), 0.0), 8) * specularStrength;
    fragColor = (spec + diff + 0.3) * texture(tex, fragTexCoord);
}