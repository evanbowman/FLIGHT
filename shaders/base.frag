#version 330 core

uniform sampler2D tex;

in vec2 TexCoord;

out vec4 fragColor;

void main() {
     //fragColor = vec4(1, 1, 1, 1);
     fragColor = fragColor = texture(tex, TexCoord);
}