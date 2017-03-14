#version 330 core

in vec2 fragTexCoord;

uniform sampler2D tex;

out vec4 fragColor;

uniform vec4 textColor;

void main() {
    vec4 sampled = vec4(1.f, 1.f, 1.f, texture(tex, fragTexCoord).r);
    fragColor = textColor * sampled;
}
