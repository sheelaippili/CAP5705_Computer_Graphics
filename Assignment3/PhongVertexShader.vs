//Phong Vertex shader
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertex_colour;
layout (location = 2) in vec3 aNormal;

out vec3 colour;
out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    colour = vertex_colour;
    FragPos = vec3(view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    LightPos = vec3(view * vec4(lightPos, 1.0));
}
