#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;
int a = 1;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords;
void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4 (aPos.x, aPos.y, aPos.z, 1.0);
}


