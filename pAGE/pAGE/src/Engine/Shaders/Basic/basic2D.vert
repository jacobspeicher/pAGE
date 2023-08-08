#version 330 core

layout (location=0) in vec3 vPos;
layout (location=1) in vec3 vColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 color;

out vec4 vertexColor;

void main() {
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	vertexColor = vec4(vColor * color, 1.0);
}