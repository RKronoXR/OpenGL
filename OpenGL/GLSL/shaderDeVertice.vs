#version 330 core
layout (location = 0) in vec3 pPosicion;

uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

void main()
{
   gl_Position = proyeccion * vista * modelo * vec4(pPosicion, 1.0);
}