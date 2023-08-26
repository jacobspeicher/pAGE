#version 330 core

layout (location=0) in vec3 vPos;
layout (location=1) in vec2 vTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 selColor;

out vec2 TexCoords;
out vec3 SelColor;

void main() {
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	TexCoords = vTexCoords;
	SelColor = selColor;
}