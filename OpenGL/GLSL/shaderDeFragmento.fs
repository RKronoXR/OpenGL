#version 330 core

uniform vec3 color;

out vec4 oColor;

void main()
{
   oColor = vec4(color, 1.f);
}