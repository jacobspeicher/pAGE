#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;

out vec4 FragColor;

vec4 CalcDirLight(DirLight light, vec3 Normal, vec3 viewDir);

void main() {
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = result;
}

vec4 CalcDirLight(DirLight light, vec3 Normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    // combine results
    vec4 ambient = vec4(light.ambient, 1.0f) * vec4(texture(material.diffuse, TexCoords));
    vec4 diffuse = vec4(light.diffuse, 1.0f) * diff * vec4(texture(material.diffuse, TexCoords));

    return (vec4(ambient + diffuse));
}