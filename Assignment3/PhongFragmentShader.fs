//Phong Fragment shader
#version 330 core
out vec4 FragColor;

in vec3 colour;
in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3 lightColor;

void main()
{
    // ambient
    vec3 objColor = vec3(0.4f, 0.0f, 0.0f);
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;
    
    
     // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    
    // specular
    float specularStrength = 0.9;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objColor;

    FragColor = vec4(result, 1.0);
}
