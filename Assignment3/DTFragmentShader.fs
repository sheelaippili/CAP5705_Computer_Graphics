#version 330 core

float near = 0.1;
float far  = 6.0;

in vec3 colour;
in vec4 VertexPos;
out vec4 FragColor;
void main()
{
    float z = VertexPos.z;
    z = z/far;
    FragColor = vec4(vec3(1.0-z), 1.0f);
    //FragColor = vec4(vec3(z), 1.0f);
}


