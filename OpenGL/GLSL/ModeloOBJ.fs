#version 330 core
out vec4 oColor;

in vec2 CoordTextura;

uniform sampler2D textura_difusa1;

void main()
{    
    oColor = texture(textura_difusa1, CoordTextura);
}