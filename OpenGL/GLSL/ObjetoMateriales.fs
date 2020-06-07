#version 330 core

out vec4 oColor;

in vec3 Posicion;
in vec3 Normal;

struct Material {
    //Colores (reflectancia) bajo las diferentes condiciones
    vec3 ambiente;
    vec3 difuso;
    vec3 especular;
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
    //Iluminacion Difusa
    vec3 normal = normalize(Normal);
    vec3 dirLuz = normalize(luz.posicion - Posicion);
    float cos_teta = max(dot(normal, dirLuz), 0.0);
    vec3 ilumDifusa = luz.difusa * cos_teta;
    //Iluminacion Especular
    vec3 dirVista = normalize(posVista - Posicion);
    vec3 dirReflexion = reflect(-dirLuz, normal);
    float cos_phi = pow(max(dot(dirVista, dirReflexion), 0.0), material.brillo);
    vec3 ilumEspecular = luz.especular * cos_phi;
    //Iluminacion Total
    vec3 colorReflejado = ilumAmbiente * material.ambiente + ilumDifusa * material.difuso + ilumEspecular * material.especular;
    //Color Final: Ya no multilplicamos por el color, pues en cada 
    //iluminación separada hemos multiplicado por el color
    oColor = vec4(colorReflejado, 1);
}