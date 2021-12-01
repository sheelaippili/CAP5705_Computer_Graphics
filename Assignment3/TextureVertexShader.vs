#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_colour;
layout (location = 3) in vec2 aTexCoord;
int a = 1;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 colour;
out vec2 TexCoord;
void main()
{
    
    gl_Position = projection * view * model * vec4 (aPos.x, aPos.y, aPos.z, 1.0);
    colour = vertex_colour;
    TexCoord = aTexCoord;
}

