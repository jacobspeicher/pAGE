#version 330 core

layout (location=0) in vec3 vPos;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 selColor;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 SelColor;

void main() {
    gl_Position = projection * view * model * vec4(vPos, 1.0f);
    FragPos = vec3(model * vec4(vPos, 1.0f));
    Normal = mat3(transpose(inverse(model))) * vNormal;
    TexCoords = vTexCoords;
    SelColor = selColor;
}