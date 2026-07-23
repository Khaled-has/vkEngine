#version 460 core

layout (location = 0) out vec4 out_Color;
layout (location = 0) in vec3 mColor;

void main()
{
    out_Color = vec4(mColor, 1.0f);
}