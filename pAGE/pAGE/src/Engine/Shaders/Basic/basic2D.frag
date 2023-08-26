#version 330 core

uniform sampler2D sprite;

out vec4 FragColor;
in vec2 TexCoords;

void main() {
	FragColor = vec4(texture(sprite, TexCoords));
}