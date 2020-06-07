#version 330 core

out vec4 oColor;

in vec3 Posicion;
in vec3 Normal;
in vec2 TexturaCoordenadas;

struct Material {
    sampler2D difuso;
    sampler2D especular;
    float brillo;
}; 

struct Luz {
    vec3 posicion;
    //Colores (simulación de espectro) bajo las diferentes condiciones
    vec3 ambiente;
    vec3 difusa;
    vec3 especular;
};

uniform vec3 posVista;
uniform Luz luz;
uniform Material material;

void main()
{
    //Iluminacion Ambiente
    vec3 ilumAmbiente = luz.ambiente;
    vec3 colorAmbiente = ilumAmbiente * vec3(texture(material.difuso, TexturaCoordenadas));
    //Iluminacion Difusa
    vec3 normal = normalize(Normal);
    vec3 dirLuz = normalize(luz.posicion - Posicion);
    float cos_teta = max(dot(normal, dirLuz), 0.0);
    vec3 ilumDifusa = luz.difusa * cos_teta;
    vec3 colorDifuso = ilumDifusa * vec3(texture(material.difuso, TexturaCoordenadas));
    //Iluminacion Especular
    vec3 dirVista = normalize(posVista - Posicion);
    vec3 dirReflexion = reflect(-dirLuz, normal);
    float cos_phi = pow(max(dot(dirVista, dirReflexion), 0.0), material.brillo);
    vec3 ilumEspecular = luz.especular * cos_phi;
    vec3 colorEspecular = ilumEspecular * vec3(texture(material.especular, TexturaCoordenadas));
    //Iluminacion Total
    vec3 colorReflejado = colorAmbiente + colorDifuso + colorEspecular;
    oColor = vec4(colorReflejado, 1);
}