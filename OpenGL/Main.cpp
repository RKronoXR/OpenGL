#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graficos.hpp"

using namespace std;

int main() {
	setlocale(LC_ALL, "es");
	ComputacionGrafica::controlador.inicializar("Computacion Grafica", "Config/OFFyOBJ.json");
	return 0;
}