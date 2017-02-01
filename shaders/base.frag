#version 330 core

uniform sampler2D tex;
uniform vec3 lightPos;
uniform vec3 lightColor;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 fragColor;

void main() {
    float ambientStrength = 0.1f;
    vec3 ambientLight = lightColor * ambientStrength;
    
    fragColor = texture(tex, fragTexCoord);
}