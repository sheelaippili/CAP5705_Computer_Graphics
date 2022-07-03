#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;
//layout (location = 2) in mat4 instanceMatrix;
layout (location = 2) in vec3 aOffset;

int a = 1;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords;

void main()
{
    //vec3 pos = aPos * (gl_InstanceID / 100.0);
    gl_Position = view * projection * vec4(aPos + aOffset, 1.0);
    TexCoords = aTexCoords;
}

