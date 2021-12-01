#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertex_colour;
//int a = 1;
//uniform mat4 mvp; // our matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 colour;
out vec4 VertexPos;
void main()
{
    colour = vertex_colour;
   // gl_Position = vec4(aPos.x, aPos.y, aPos.z, 2.0);
   //gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    //gl_Position = proj * view * vec4 (vertex_position, 2.0);
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    VertexPos = gl_Position;
    
}
