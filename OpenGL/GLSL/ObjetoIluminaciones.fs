#version 330 core

// Considere que este código puede ser redundante para ser mas claro

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

vec3 calcularReflejoLuzDireccionada(LuzDireccionada luzDireccionada, vec3 normal, vec3 dirVista, vec3 mat_difuso, vec3 mat_especular, float mat_brillo);
vec3 calcularReflejoLuzPuntual(LuzPuntual luzPuntual, vec3 normal, vec3 posicion, vec3 dirVista, vec3 mat_difuso, vec3 mat_especular, float mat_brillo);
vec3 calcularReflejoLuzSpot(LuzSpot luzSpot, vec3 normal, vec3 posicion, vec3 dirVista, vec3 mat_difuso, vec3 mat_especular, float mat_brillo);

void main()
{
    // Propiedades
    vec3 normal = normalize(Normal);
    vec3 dirVista = normalize(posVista - Posicion);
    // Convirtiendo las propiedades del material
    vec3 mat_difuso = vec3(texture(material.difuso, CoordenadasTextura));
    vec3 mat_especular = vec3(texture(material.especular, CoordenadasTextura));
    float mat_brillo = material.brillo;
    //Calculando el reflejo
    vec3 colorReflejado = vec3(0,0,0);
    // Luz direccional
    colorReflejado += calcularReflejoLuzDireccionada(luzDireccionada, normal, dirVista, mat_difuso, mat_especular, mat_brillo);
    // Luces Puntuales
    colorReflejado += calcularReflejoLuzPuntual(luzPuntual, normal, Posicion, dirVista, mat_difuso, mat_especular, mat_brillo);
    // Luz Spot
    colorReflejado += calcularReflejoLuzSpot(luzSpot, normal, Posicion, dirVista, mat_difuso, mat_especular, mat_brillo);
    
    oColor = vec4(colorReflejado, 1.0);
}

vec3 calcularReflejoLuzDireccionada(LuzDireccionada luzDireccionada, vec3 normal, vec3 dirVista, vec3 mat_difuso, vec3 mat_especular, float mat_brillo)
{
    vec3 dirLuz = normalize(-luzDireccionada.direccion);
    // Reflejo Ambiente
    vec3 ilumAmbiente = luzDireccionada.color.ambiente;
    vec3 reflejoAmbiente = ilumAmbiente * mat_difuso;
    // Reflejo Difuso
    vec3 ilumDifusa = luzDireccionada.color.difuso * max(dot(normal, dirLuz), 0.0);//luz x cos(teta)
    vec3 reflejoDifuso = ilumDifusa *  mat_difuso;
    // Reflejo Especular
    vec3 dirReflejo = reflect(-dirLuz, normal);//reflect=I-2.0xN.IxN donde I:-dirLuz, N:normal , .: producto escalar
    vec3 ilumEspecular = luzDireccionada.color.especular * pow(max(dot(dirVista, dirReflejo), 0.0), mat_brillo);
    vec3 reflejoEspecular = ilumEspecular * mat_especular;
    // Combinando resultados
    return (reflejoAmbiente + reflejoDifuso + reflejoEspecular);
}

vec3 calcularReflejoLuzPuntual(LuzPuntual luzPuntual, vec3 normal, vec3 posicion, vec3 dirVista, vec3 mat_difuso, vec3 mat_especular, float mat_brillo)
{
    vec3 dirLuz = normalize(luzPuntual.posicion - posicion);
    // Atenuacion
    float distancia = length(luzPuntual.posicion - posicion);
    float atenuacion = 1.0 / (luzPuntual.constAtenuacion.constante + 
                     luzPuntual.constAtenuacion.linear * distancia + 
                     luzPuntual.constAtenuacion.cuadratica * (distancia * distancia));
    //Reflejo Ambiente
    vec3 ilumAmbiente = luzPuntual.color.ambiente;
    ilumAmbiente *= atenuacion;
    vec3 reflejoAmbiente = ilumAmbiente * mat_difuso;
    //Reflejo Difuso
    vec3 ilumDifuso = luzPuntual.color.difuso * max(dot(normal, dirLuz), 0.0);//luz x cos(teta)
    ilumDifuso = ilumDifuso * atenuacion;
    vec3 reflejoDifuso = ilumDifuso * mat_difuso;
    //Reflejo Especular
    vec3 dirReflejo = reflect(-dirLuz, normal);//reflect=I-2.0xN·IxN donde I:-dirLuz, N:normal
    vec3 ilumEspecular = luzPuntual.color.especular * pow(max(dot(dirVista, dirReflejo), 0.0), mat_brillo);//luz x pow(cos(phi),brillo)
    ilumEspecular = ilumEspecular * atenuacion;
    vec3 reflejoEspecular = ilumEspecular * mat_especular;
    // Sumando los reflejos: ambiente + difuso + especular
    return (reflejoAmbiente + reflejoDifuso + reflejoEspecular);
}

vec3 calcularReflejoLuzSpot(LuzSpot luzSpot, vec3 normal, vec3 posicion, vec3 dirVista, vec3 mat_difuso, vec3 mat_especular, float mat_brillo)
{
    vec3 dirLuz = normalize(luzSpot.posicion - posicion);
    // Atenuacion
    float distancia = length(luzPuntual.posicion - posicion);
    float atenuacion = 1.0 / (luzPuntual.constAtenuacion.constante + 
                     luzPuntual.constAtenuacion.linear * distancia + 
                     luzPuntual.constAtenuacion.cuadratica * (distancia * distancia));
    // spotlight intensity
    float cos_alfa = dot(dirLuz, normalize(-luzSpot.direccion)); 
    float epsilon = luzSpot.anguloInterno - luzSpot.anguloExterno;
    float intensidad = clamp((cos_alfa - luzSpot.anguloExterno) / epsilon, 0.0, 1.0);
    //Reflejo Ambiente
    vec3 ilumAmbiente = luzSpot.color.ambiente;
    ilumAmbiente *= atenuacion * intensidad;
    vec3 reflejoAmbiente = ilumAmbiente * mat_difuso;
    //Reflejo Difuso
    vec3 ilumDifusa = luzSpot.color.difuso * max(dot(normal, dirLuz), 0.0);
    ilumDifusa *= atenuacion * intensidad;
    vec3 reflejoDifuso = ilumDifusa * mat_difuso;
    //Reflejo Especular
    vec3 dirReflejo = reflect(-dirLuz, normal);
    vec3 ilumEspecular = luzSpot.color.especular * pow(max(dot(dirVista, dirReflejo), 0.0), mat_brillo);
    ilumEspecular *= atenuacion * intensidad;
    vec3 reflejoEspecular = ilumEspecular * mat_especular;
    // Sumando los reflejos: ambiente + difuso + especular
    return (reflejoAmbiente + reflejoDifuso + reflejoEspecular);
}