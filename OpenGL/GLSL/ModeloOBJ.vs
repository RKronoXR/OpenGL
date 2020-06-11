#version 330 core
layout (location = 0) in vec3 pPosicion;
layout (location = 1) in vec3 pNormal;
layout (location = 2) in vec2 pCoordTextura;

out vec2 CoordTextura;

uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

void main()
{
    CoordTextura = pCoordTextura;    
    gl_Position = proyeccion * vista * modelo * vec4(pPosicion, 1.0);
}