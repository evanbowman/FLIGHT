#version 330 core

in vec2 fragTexCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
    float a = texture(tex, fragTexCoord).r;
    fragColor = vec4(1.f, 1.f, 1.f, a);
}
