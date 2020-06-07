#version 330 core

out vec4 oColor;

in vec3 Posicion;
in vec3 Normal;

uniform vec3 posLuz;
uniform vec3 posVista;
uniform vec3 colorLuz;
uniform vec3 color;

void main()
{
    //Iluminacion Ambiente
    float intensidadAmbiente = 0.5;
    vec3 ilumAmbiente = colorLuz * intensidadAmbiente;
    //Iluminacion Difusa
    vec3 normal = normalize(Normal);
    vec3 dirLuz = normalize(posLuz - Posicion);
    float cos_teta = max(dot(normal, dirLuz), 0.0);
    vec3 ilumDifusa = colorLuz * cos_teta;
    //Iluminacion Especular
    vec3 dirVista = normalize(posVista - Posicion);
    vec3 dirReflexion = reflect(-dirLuz, normal);
    float s = 32;
    float cos_phi = pow(max(dot(dirVista, dirReflexion), 0.0), s);
    vec3 ilumEspecular = colorLuz * cos_phi;
    //Iluminacion Total
    vec3 ilumTotal = ilumAmbiente + ilumDifusa + ilumEspecular;
    //Color Final
    oColor = vec4(ilumTotal*color, 1);
}