#version 330 core

layout (location=0) in vec3 vPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

out vec4 vertexColor;

void main() {
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	vertexColor = vec4(color.xyz, 0.01f);
}