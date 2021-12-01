//Phong Vertex shader
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertex_colour;
layout (location = 2) in vec3 aNormal;

out vec3 LightingColor;
out vec3 colour;

uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform mat4 model;
uniform mat4 view;


void main()
{
    
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    colour = vertex_colour;

    vec3 Position = vec3(model * vec4(aPos.x, aPos.y, aPos.z, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // ambient lighting
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
      
    // diffuse lighting
    float diffuseStrength = 0.8;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;
    
    
    // specular lighting
    float specularStrength = 1.0;
    //vec3 viewDir = normalize(viewPos - Position);
    vec3 viewDir = normalize(-Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
     

    LightingColor = ambient + diffuse + specular;
}




