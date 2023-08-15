#version 330 core

out vec4 FragColor;
in vec4 vertexColor;

void main() {
	FragColor = vertexColor;
	FragColor.a *= 0.5f;
}