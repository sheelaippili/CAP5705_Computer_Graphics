//
//  TextureFragmentShader.cpp
//  Assignment3
//
//  Created by Sheela Ippili on 11/19/21.
//

#include <stdio.h>

#version 330 core
out vec4 FragColor;
  
in vec3 colour;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}

