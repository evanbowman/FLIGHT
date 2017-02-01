#version 330 core

uniform sampler2D tex;
uniform vec3 lightPos;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(fragNormal);
    // we're outdoors, light comes from the sky
    vec3 lightDir = vec3(0, 1, 0);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    fragColor = (diff + 0.3) * texture(tex, fragTexCoord);
}