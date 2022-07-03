#version 330 core
out vec4 FragColor;
  
in vec3 colour;
in vec3 textureDir;
in vec2 bluffTexCoord;

uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, textureDir);
}

