#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 SelectionColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 SelColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;

vec4 CalcDirLight(DirLight light, vec3 Normal, vec3 viewDir);

void main() {
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = result;
    SelectionColor = vec4(SelColor, 1.0f);
}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    // combine results
    vec4 ambient = vec4(light.ambient, 1.0f) * vec4(texture(material.texture_diffuse1, TexCoords));
    vec4 diffuse = vec4(light.diffuse, 1.0f) * diff * vec4(texture(material.texture_diffuse1, TexCoords));

    return (vec4(ambient + diffuse));
}