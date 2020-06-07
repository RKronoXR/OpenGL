#version 330 core

out vec4 oColor;

in vec3 Posicion;
in vec3 Normal;
in vec2 CoordenadasTextura;

struct Material {
    sampler2D difuso;
    sampler2D especular;
    float brillo;
}; 

struct ColorLuz {
    vec3 ambiente;
    vec3 difuso;
    vec3 especular;
};

struct ConstAtenuacion{
    float constante;
    float linear;
    float cuadratica;
};

struct LuzDireccionada {
    vec3 direccion;
    ColorLuz color;
};

struct LuzPuntual {
    vec3 posicion;
    ColorLuz color;
    ConstAtenuacion constAtenuacion;
};

struct LuzSpot {
    vec3 posicion;
    vec3 direccion;
    ColorLuz color;
    ConstAtenuacion constAtenuacion;
    float anguloInterno;
    float anguloExterno;
};

uniform vec3 posVista;
uniform LuzDireccionada luzDireccionada;
uniform LuzPuntual luzPuntual;
uniform LuzSpot luzSpot;
uniform Material material;

vec3 calcularReflejoLuzDireccionada(LuzDireccionada luzDireccionada, vec3 normal, vec3 dirVista);
vec3 calcularReflejoLuzPuntual(LuzPuntual luzPuntual, vec3 normal, vec3 posicion, vec3 dirVista);
vec3 calcularReflejoLuzSpot(LuzSpot luzSpot, vec3 normal, vec3 posicion, vec3 dirVista);

void main()
{
    // Propiedades
    vec3 normal = normalize(Normal);
    vec3 dirVista = normalize(posVista - Posicion);
    //Calculando el reflejo
    vec3 colorReflejado = vec3(0,0,0);
    // Luz direccional
    colorReflejado += calcularReflejoLuzDireccionada(luzDireccionada, normal, dirVista);
    // Luces Puntuales
    colorReflejado += calcularReflejoLuzPuntual(luzPuntual, normal, Posicion, dirVista);
    // Luz Spot
    colorReflejado += calcularReflejoLuzSpot(luzSpot, normal, Posicion, dirVista);    
    
    oColor = vec4(colorReflejado, 1.0);
}

vec3 calcularReflejoLuzDireccionada(LuzDireccionada luzDireccionada, vec3 normal, vec3 dirVista)
{
    vec3 dirLuz = normalize(-luzDireccionada.direccion);
    // Reflejo Ambiente
    vec3 reflejoAmbiente = luzDireccionada.color.ambiente * vec3(texture(material.difuso, CoordenadasTextura));
    // Reflejo Difuso
    float cos_teta = max(dot(normal, dirLuz), 0.0);
    vec3 ilumDifusa = luzDireccionada.color.difuso * cos_teta;
    vec3 reflejoDifuso = ilumDifusa *  vec3(texture(material.difuso, CoordenadasTextura));
    // Reflejo Especular
    vec3 dirReflejo = reflect(-dirLuz, normal);
    float cos_phi = pow(max(dot(dirVista, dirReflejo), 0.0), material.brillo);
    vec3 reflejoEspecular = luzDireccionada.color.especular * cos_phi * vec3(texture(material.especular, CoordenadasTextura));
    // Combinando resultados
    return (reflejoAmbiente + reflejoDifuso + reflejoEspecular);
}

vec3 calcularReflejoLuzPuntual(LuzPuntual luzPuntual, vec3 normal, vec3 posicion, vec3 dirVista)
{
    vec3 dirLuz = normalize(luzPuntual.posicion - posicion);
    // Shading difuso
    float cos_teta = max(dot(normal, dirLuz), 0.0);
    // Shading especular
    vec3 dirReflejo = reflect(-dirLuz, normal);
    float cos_phi = pow(max(dot(dirVista, dirReflejo), 0.0), material.brillo);
    // Atenuacion
    float distancia = length(luzPuntual.posicion - posicion);
    float atenuacion = 1.0 / (luzPuntual.constAtenuacion.constante + luzPuntual.constAtenuacion.linear * distancia + luzPuntual.constAtenuacion.cuadratica * (distancia * distancia));
    // Combinando resultados
    vec3 reflejoAmbiente = luzPuntual.color.ambiente * vec3(texture(material.difuso, CoordenadasTextura));
    vec3 reflejoDifuso = luzPuntual.color.difuso * cos_teta * vec3(texture(material.difuso, CoordenadasTextura));
    vec3 reflejoEspecular = luzPuntual.color.especular * cos_phi * vec3(texture(material.especular, CoordenadasTextura));
    // Atenuando los reflejos
    reflejoAmbiente *= atenuacion;
    reflejoDifuso *= atenuacion;
    reflejoEspecular *= atenuacion;
    return (reflejoAmbiente + reflejoDifuso + reflejoEspecular);
}

vec3 calcularReflejoLuzSpot(LuzSpot luzSpot, vec3 normal, vec3 posicion, vec3 dirVista)
{
    vec3 dirLuz = normalize(luzSpot.posicion - posicion);
    // Shading difuso
    float cos_teta = max(dot(normal, dirLuz), 0.0);
    // Shading especular
    vec3 dirReflejo = reflect(-dirLuz, normal);
    float cos_phi = pow(max(dot(dirVista, dirReflejo), 0.0), material.brillo);
    // Atenuacion
    float distancia = length(luzSpot.posicion - posicion);
    float atenuacion = 1.0 / (luzSpot.constAtenuacion.constante + luzSpot.constAtenuacion.linear * distancia + luzSpot.constAtenuacion.cuadratica * (distancia * distancia));
    // spotlight intensity
    float cos_alfa = dot(dirLuz, normalize(-luzSpot.direccion)); 
    float epsilon = luzSpot.anguloInterno - luzSpot.anguloExterno;
    float intensidad = clamp((cos_alfa - luzSpot.anguloExterno) / epsilon, 0.0, 1.0);
    // Combinando resultados
    vec3 reflejoAmbiente = luzSpot.color.ambiente * vec3(texture(material.difuso, CoordenadasTextura));
    vec3 reflejoDifuso = luzSpot.color.difuso * cos_teta * vec3(texture(material.difuso, CoordenadasTextura));
    vec3 reflejoEspecular = luzSpot.color.especular * cos_phi * vec3(texture(material.especular, CoordenadasTextura));
    // Atenuando los reflejos
    reflejoAmbiente *= atenuacion * intensidad;
    reflejoDifuso *= atenuacion * intensidad;
    reflejoEspecular *= atenuacion * intensidad;
    return (reflejoAmbiente + reflejoDifuso + reflejoEspecular);
}