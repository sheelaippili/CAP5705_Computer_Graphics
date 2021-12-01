#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_colour;
int a = 1;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 colour;
void main()
{
    colour = vertex_colour;
    gl_Position = projection * view * model * vec4 (aPos.x, aPos.y, aPos.z, 1.0);
}
