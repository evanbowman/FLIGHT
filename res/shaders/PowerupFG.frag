#version 330 core

const vec3 color = vec3(0.949, 0.765, 0.027);

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 fragColor;

uniform float fill;

float l;
float epsilon;

uniform sampler2D tex;

float Circle(float rad) {
    return smoothstep(rad, rad - epsilon, l);
}

void main() {
    float opacity = 1.f;
    l = length(fragTexCoord - vec2(0.5, 0.5));
    epsilon = fwidth(l);
    float circ = Circle(0.42);
    float icon = texture(tex, fragTexCoord).a;
    fragColor = vec4(0, 0, 0, 0);
    if (circ > 0.f) {
        if (fragTexCoord.y < fill) {
            fragColor = vec4(color, circ * (1.f - icon));
        } else {
            fragColor = vec4(color, icon);
        }
    }
}
