#version 330 core

out vec4 FragColor;
in vec4 vertexColor;

void main() {
	// FragColor = vertexColor;
	FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}