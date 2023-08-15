#version 330 core

layout (location=0) in vec3 vPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 color;

out VS_OUT {
    vec3 color;
} vs_out;

void main() {
	gl_Position = projection * view * model * vec4(vPos, 1.0);
    vs_out.color = color;
}