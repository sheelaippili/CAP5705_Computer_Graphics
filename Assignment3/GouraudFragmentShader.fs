//Gouroud Fragment shader
#version 330 core
out vec4 FragColor;

in vec3 colour;

in vec3 LightingColor;

void main()
{
    
    FragColor = vec4(LightingColor * vec3(0.4f, 0.0f, 0.0f), 1.0);
}
