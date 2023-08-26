#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 SelectionColor;

uniform sampler2D sprite;

in vec2 TexCoords;
in vec3 SelColor;

void main() {
	FragColor = vec4(texture(sprite, TexCoords));
	SelectionColor = vec4(SelColor, 1.0f);
}