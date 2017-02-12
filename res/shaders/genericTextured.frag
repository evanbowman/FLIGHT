#version 330 core

in vec2 fragTexCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
    fragColor = texture(tex, fragTexCoord);
}
