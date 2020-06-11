#ifndef __GRAFICOS_HPP__
#define __GRAFICOS_HPP__

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <json/json.h>
#include <glm/gtc/matrix_transform.hpp>

namespace ComputacionGrafica {

	using namespace std;
	using namespace glm;
	using namespace Json;

	void redimensionarBuffer(GLFWwindow* ventana, int ancho, int alto);
	void desplazarCamara(GLFWwindow* ventana);
	void girarCamara(GLFWwindow* window, double x, double y);
	void acercarCamara(GLFWwindow* window, double dx, double dZoom);

	enum TransfGeometrica { Traslacion, Rotacion, Escalamiento };
	enum MovimientoCamara { ADELANTE = 'W', ATRAS = 'S', IZQUIERDA = 'A', DERECHA = 'D' };
	typedef struct InfoMaterial {
		string nombre;
		vec3  ambiente;
		vec3  difuso;
		vec3  especular;
		float brillo;
		InfoMaterial(string nombre, vec3 ambiente, vec3 difuso, vec3 especular, float brillo) {
			this->nombre = nombre;
			this->ambiente = ambiente;
			this->difuso = difuso;
			this->especular = especular;
			this->brillo = brillo;
		}
	};
	typedef struct InfoConstAtenuacion {
		float constante, linear, cuadratica;
		InfoConstAtenuacion(float constante, float linear, float cuadratica) {
			this->constante = constante;
			this->linear = linear;
			this->cuadratica = cuadratica;
		}
	};
	typedef struct { vec3 ambiente; vec3 difuso; vec3 especular; } ColorLuz;
	struct InfoLuz {
		string tipo;
		ColorLuz color;
		InfoLuz(string tipo) {
			this->tipo = tipo;
		}
	};
	struct InfoLuzDireccionada : public InfoLuz {
	public:
		vec3 direccion;
		InfoLuzDireccionada() : InfoLuz("Direccionada") { }
	};
	struct InfoLuzPuntual : public InfoLuz {
	public:
		vec3 posicion;
		InfoConstAtenuacion* constantesAtenuacion;
		InfoLuzPuntual() : InfoLuz("Puntual") { }
	};
	struct InfoLuzFocal : public InfoLuz {
	public:
		vec3 posicion, direccion;
		InfoConstAtenuacion* constantesAtenuacion;
		float anguloInterno, anguloExterno;
		InfoLuzFocal() : InfoLuz("Focal") { }
	};
	class CInfoConstantes {
		typedef unordered_map<string, InfoMaterial*> InfoMateriales;
		typedef unordered_map<unsigned short, InfoConstAtenuacion*> InfoConstDistancias;
		InfoMateriales infoMateriales;
		InfoConstDistancias infoConstDistancias;
	public:
		CInfoConstantes() {
			this->inicializarInfoMateriales();
			this->inicializarInfoAtenuacion();
		}
		~CInfoConstantes() {
			for (auto objInfoMaterial : this->infoMateriales) {
				InfoMaterial* infoMaterial = (InfoMaterial*)objInfoMaterial.second;
				delete infoMaterial;
			}
			for (auto objInfoConstDistancia : this->infoConstDistancias) {
				InfoConstAtenuacion* infoConstDistancia = (InfoConstAtenuacion*)objInfoConstDistancia.second;
				delete infoConstDistancia;
			}
		}
		InfoMaterial* getInfoMaterial(string nombre) {
			//if (this->infoMateriales.find(nombre) == this->infoMateriales.end())
			return this->infoMateriales[nombre];
			//else
				//return this->infoMateriales["Ninguno"];
		}
		InfoConstAtenuacion* getInfoConstAtenuacion(short distancia) {
			//if (this->infoConstDistancias.find(distancia) == this->infoConstDistancias.end())
			return this->infoConstDistancias[distancia];
			//else
				//return this->infoConstDistancias[7];
		}
	private:
		void inicializarInfoMateriales() {
			//Información obtenida de: http://devernay.free.fr/cours/opengl/materials.html
			this->infoMateriales["Esmeralda"] = new InfoMaterial("Esmeralda", vec3(0.0215f, 0.1745f, 0.0215f), vec3(0.07568f, 0.61424f, 0.07568f), vec3(0.633f, 0.727811f, 0.633f), 0.6f);
			this->infoMateriales["Jade"] = new InfoMaterial("Jade", vec3(0.135f, 0.2225f, 0.1575f), vec3(0.54f, 0.89f, 0.63f), vec3(0.316228f, 0.316228f, 0.316228f), 0.1f);
			this->infoMateriales["Obsidian"] = new InfoMaterial("Obsidian", vec3(0.05375f, 0.05f, 0.06625f), vec3(0.18275f, 0.17f, 0.22525f), vec3(0.332741f, 0.328634f, 0.346435f), 0.3f);
			this->infoMateriales["Perla"] = new InfoMaterial("Perla", vec3(0.25f, 0.20725f, 0.20725f), vec3(1.f, 0.829f, 0.829f), vec3(0.296648f, 0.296648f, 0.296648f), 0.088f);
			this->infoMateriales["Rubí"] = new InfoMaterial("Rubí", vec3(0.1745f, 0.01175f, 0.01175f), vec3(0.61424f, 0.04136f, 0.04136f), vec3(0.727811f, 0.626959f, 0.626959f), 0.6f);
			this->infoMateriales["Turquesa"] = new InfoMaterial("Turquesa", vec3(0.1f, 0.18725f, 0.1745f), vec3(0.396f, 0.74151f, 0.69102f), vec3(0.297254f, 0.30829f, 0.306678f), 0.1f);
			this->infoMateriales["Latón"] = new InfoMaterial("Latón", vec3(0.329412f, 0.223529f, 0.027451f), vec3(0.780392f, 0.568627f, 0.113725f), vec3(0.992157f, 0.941176f, 0.807843f), 0.21794872f);
			this->infoMateriales["Bronce"] = new InfoMaterial("Bronce", vec3(0.2125f, 0.1275f, 0.054f), vec3(0.714f, 0.4284f, 0.18144f), vec3(0.393548f, 0.271906f, 0.166721f), 0.2f);
			this->infoMateriales["Cromo"] = new InfoMaterial("Cromo", vec3(0.25f, 0.25f, 0.25f), vec3(0.4f, 0.4f, 0.4f), vec3(0.774597f, 0.774597f, 0.774597f), 0.6f);
			this->infoMateriales["Cobre"] = new InfoMaterial("Cobre", vec3(0.19125f, 0.0735f, 0.0225f), vec3(0.7038f, 0.27048f, 0.0828f), vec3(0.256777f, 0.137622f, 0.086014f), 0.1f);
			this->infoMateriales["Oro"] = new InfoMaterial("Oro", vec3(0.24725f, 0.1995f, 0.0745f), vec3(0.75164f, 0.60648f, 0.22648f), vec3(0.628281f, 0.555802f, 0.366065f), 0.4f);
			this->infoMateriales["Plata"] = new InfoMaterial("Plata", vec3(0.19225f, 0.19225f, 0.19225f), vec3(0.50754f, 0.50754f, 0.50754f), vec3(0.508273f, 0.508273f, 0.508273f), 0.4f);
			this->infoMateriales["Plástico Negro"] = new InfoMaterial("Plástico Negro", vec3(0.0f, 0.0f, 0.0f), vec3(0.01f, 0.01f, 0.01f), vec3(0.50f, 0.50f, 0.50f), .25f);
			this->infoMateriales["Plástico Cian"] = new InfoMaterial("Plástico Cian", vec3(0.0f, 0.1f, 0.06f), vec3(0.0f, 0.50980392f, 0.50980392f), vec3(0.50196078f, 0.50196078f, 0.50196078f), .25f);
			this->infoMateriales["Plástico Verde"] = new InfoMaterial("Plástico Verde", vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.35f, 0.1f), vec3(0.45f, 0.55f, 0.45f), .25f);
			this->infoMateriales["Plástico Rojo"] = new InfoMaterial("Plástico Rojo", vec3(0.0f, 0.0f, 0.0f), vec3(0.5f, 0.0f, 0.0f), vec3(0.7f, 0.6f, 0.6f), .25f);
			this->infoMateriales["Plástico Blanco"] = new InfoMaterial("Plástico Blanco", vec3(0.0f, 0.0f, 0.0f), vec3(0.55f, 0.55f, 0.55f), vec3(0.70f, 0.70f, 0.70f), .25f);
			this->infoMateriales["Plástico Amarillo"] = new InfoMaterial("Plástico Amarillo", vec3(0.0f, 0.0f, 0.0f), vec3(0.5f, 0.5f, 0.0f), vec3(0.60f, 0.60f, 0.50f), .25f);
			this->infoMateriales["Caucho Negro"] = new InfoMaterial("Caucho Negro", vec3(0.02f, 0.02f, 0.02f), vec3(0.01f, 0.01f, 0.01f), vec3(0.4f, 0.4f, 0.4f), .078125f);
			this->infoMateriales["Caucho Cian"] = new InfoMaterial("Caucho Cian", vec3(0.0f, 0.05f, 0.05f), vec3(0.4f, 0.5f, 0.5f), vec3(0.04f, 0.7f, 0.7f), .078125f);
			this->infoMateriales["Caucho Verde"] = new InfoMaterial("Caucho Verde", vec3(0.0f, 0.05f, 0.0f), vec3(0.4f, 0.5f, 0.4f), vec3(0.04f, 0.7f, 0.04f), .078125f);
			this->infoMateriales["Caucho Rojo"] = new InfoMaterial("Caucho Rojo", vec3(0.05f, 0.0f, 0.0f), vec3(0.5f, 0.4f, 0.4f), vec3(0.7f, 0.04f, 0.04f), .078125f);
			this->infoMateriales["Caucho Blanco"] = new InfoMaterial("Caucho Blanco", vec3(0.05f, 0.05f, 0.05f), vec3(0.5f, 0.5f, 0.5f), vec3(0.7f, 0.7f, 0.7f), .078125f);
			this->infoMateriales["Caucho Amarillo"] = new InfoMaterial("Caucho Amarillo", vec3(0.05f, 0.05f, 0.0f), vec3(0.5f, 0.5f, 0.4f), vec3(0.7f, 0.7f, 0.04f), .078125f);
			this->infoMateriales["Ninguno"] = new InfoMaterial("Ninguno", vec3(.25f, .5f, .8f), vec3(.25f, .5f, .8f), vec3(.25f, .5f, .8f), 32.f);
		}
		void inicializarInfoAtenuacion() {
			this->infoConstDistancias[7]    = new InfoConstAtenuacion(1.f, .7f, 1.8f);
			this->infoConstDistancias[13]   = new InfoConstAtenuacion(1.f, .35f, .44f);
			this->infoConstDistancias[20]   = new InfoConstAtenuacion(1.f, .22f, .2f);
			this->infoConstDistancias[32]   = new InfoConstAtenuacion(1.f, .14f, .07f);
			this->infoConstDistancias[50]   = new InfoConstAtenuacion(1.f, .09f, .032f);
			this->infoConstDistancias[65]   = new InfoConstAtenuacion(1.f, .07f, .017f);
			this->infoConstDistancias[100]  = new InfoConstAtenuacion(1.f, .045f, .0075f);
			this->infoConstDistancias[160]  = new InfoConstAtenuacion(1.f, .027f, .0028f);
			this->infoConstDistancias[200]  = new InfoConstAtenuacion(1.f, .022f, .0019f);
			this->infoConstDistancias[325]  = new InfoConstAtenuacion(1.f, .014f, .0007f);
			this->infoConstDistancias[600]  = new InfoConstAtenuacion(1.f, .007f, .0002f);
			this->infoConstDistancias[3250] = new InfoConstAtenuacion(1.f, .0014f, .000007f);
		}
	};
	class CProgramaShaders
	{
		GLuint idPrograma;
	public:
		CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento)
		{
			// Variables para obtener leer el código de los shader de vértice y fragmento
			string   strCodigoShaderVertice;
			string   strCodigoShaderFragmento;
			ifstream pArchivoShaderVertice;
			ifstream pArchivoShaderFragmento;
			// Nos aseguramos de poder mostrar excepciones en caso haya
			pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
			pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
			try
			{
				// Abriendo los archivos de código de los shaders
				pArchivoShaderVertice.open(rutaShaderVertice);
				pArchivoShaderFragmento.open(rutaShaderFragmento);
				// Leyendo la información de los archivos 
				stringstream lectorShaderVertice, lectorShaderFragmento;
				lectorShaderVertice << pArchivoShaderVertice.rdbuf();
				lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
				// Cerrando los archivos
				pArchivoShaderVertice.close();
				pArchivoShaderFragmento.close();
				// Pasando la información leída a strings
				strCodigoShaderVertice = lectorShaderVertice.str();
				strCodigoShaderFragmento = lectorShaderFragmento.str();
			}
			catch (ifstream::failure&) { cout << "ERROR: Los archivos no puedieron ser leídos correctamente." << endl; }
			const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
			const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();
			// Asociando y compilando el código de los shaders
			GLuint idShaderVertice, idShaderFragmento;
			// Shader de Vértice
			idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(idShaderVertice, 1, &codigoShaderVertice, nullptr);
			glCompileShader(idShaderVertice);
			verificarErrores(idShaderVertice, "Vértice");
			// Shader de Fragmento
			idShaderFragmento = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(idShaderFragmento, 1, &codigoShaderFragmento, nullptr);
			glCompileShader(idShaderFragmento);
			verificarErrores(idShaderFragmento, "Fragmento");
			// Programa de Shaders
			this->idPrograma = glCreateProgram();
			glAttachShader(this->idPrograma, idShaderVertice);
			glAttachShader(this->idPrograma, idShaderFragmento);
			glLinkProgram(this->idPrograma);
			verificarErrores(this->idPrograma, "Programa");
			// Borra los shader, al estar enlazados a nuestro programa, ya no son necesarios
			glDeleteShader(idShaderVertice);
			glDeleteShader(idShaderFragmento);
		}
		~CProgramaShaders() {
			glDeleteProgram(this->idPrograma);
		}
		void usar() const
		{
			glUseProgram(this->idPrograma);
		}
		void setBool(const string &nombre, bool valor) const
		{
			glUniform1i(glGetUniformLocation(this->idPrograma, nombre.c_str()), (int)valor);
		}
		void setInt(const string &nombre, int valor) const
		{
			glUniform1i(glGetUniformLocation(this->idPrograma, nombre.c_str()), valor);
		}
		void setFloat(const string &nombre, float valor) const
		{
			glUniform1f(glGetUniformLocation(this->idPrograma, nombre.c_str()), valor);
		}
		void setVec2(const string &nombre, const glm::vec2 &valor) const
		{
			glUniform2fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
		}
		void setVec2(const string &nombre, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y);
		}
		void setVec3(const string &nombre, const glm::vec3 &valor) const
		{
			glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
		}
		void setVec3(const string &name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(this->idPrograma, name.c_str()), x, y, z);
		}
		void setVec4(const string &nombre, const glm::vec4 &valor) const
		{
			glUniform4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
		}
		void setVec4(const string &nombre, float x, float y, float z, float w) const
		{
			glUniform4f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y, z, w);
		}
		void setMat2(const string &nombre, const glm::mat2 &mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setMat3(const string &nombre, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setMat4(const string &nombre, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
	private:
		void verificarErrores(GLuint identificador, string tipo)
		{
			GLint exito;
			GLchar log[1024];
			if (tipo == "Programa")
			{
				glGetProgramiv(identificador, GL_LINK_STATUS, &exito);
				if (!exito)
				{
					glGetProgramInfoLog(identificador, 1024, nullptr, log);
					cout << "ERROR: El enlace de los shader al Programa no se llevó a cabo.\n" << log << endl;
				}
			}
			else
			{
				glGetShaderiv(identificador, GL_COMPILE_STATUS, &exito);
				if (!exito)
				{
					glGetShaderInfoLog(identificador, 1024, nullptr, log);
					cout << "ERROR: La compilación del Shader de " << tipo << " no se llevó a cabo.\n" << log << endl;
				}
			}
		}
	};
	class CTranfGeometrica {
		TransfGeometrica tipoTG;
		vec3 configuracion;
		float angulo;
	public:
		CTranfGeometrica(TransfGeometrica tipoTG, vec3 configuracion, float angulo = 0)
			: tipoTG(tipoTG), configuracion(configuracion), angulo(angulo) {}
		mat4 transformar(mat4 &modelo) {
			//TODO: cambiar a gusto la forma de calcular el ángulo
			switch (this->tipoTG) {
			case Traslacion: return translate(modelo, this->configuracion);
			case Rotacion: return rotate(modelo, float(angulo*glfwGetTime()), this->configuracion);
			case Escalamiento: return scale(modelo, this->configuracion);
			default: return modelo;
			}
		}
	};
	class CRenderizador {
		GLFWwindow* ventana;
		GLuint anchoVentana, altoVentana;
		GLuint idArregloDeObjetosVertices;
	public:
		CRenderizador(GLuint anchoVentana = 1024, GLuint altoVentana = 768)
			: anchoVentana(anchoVentana), altoVentana(altoVentana) { }
		~CRenderizador() {
			glDeleteVertexArrays(1, &idArregloDeObjetosVertices);
			glfwTerminate();
		}
		bool inicializar(const char* titulo) {
			//Inicializa glfw y considerará los la versión de opengl 3
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			//Permite tener acceso a funcionalidades de Opengl sin características de versiones anteriores que ya no necesitamos
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			//Crea la ventana donde renderizaremos (a través del viewport) y verifica su correcta creación
			ventana = glfwCreateWindow(anchoVentana, altoVentana, titulo, nullptr, nullptr);
			if (ventana == nullptr) {
				cout << "La ventana GLFW no se creó correctamente" << endl;
				glfwTerminate();
				return false;
			}
			//Hacer el contexto de la ventana, el principal contexto del proceso
			glfwMakeContextCurrent(ventana);
			//Le indicamos a GLFW que cada vez que se redimensione la ventana debe ejecutarse dicha función
			glfwSetFramebufferSizeCallback(ventana, redimensionarBuffer);
			//glfwSetKeyCallback(ventana, desplazarCamara);//Colocandolo dentro del while es mas continuo y suave el movimiento
			glfwSetCursorPosCallback(ventana, girarCamara);
			glfwSetScrollCallback(ventana, acercarCamara);
			glfwSetInputMode(ventana, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			//Verifica que GLAD se inicialice correctamente
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				cout << "Glad no se inicializó correctamente" << endl;
				return false;
			}
			//Indica a OpenGL que renderice los objetos tomando en cuenta la profundidad
			glEnable(GL_DEPTH_TEST);
			//Genera el arreglo de objetos y lo enlaza
			glGenVertexArrays(1, &this->idArregloDeObjetosVertices);
			return true;
		}
		void refrescarViewport() {
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindVertexArray(this->idArregloDeObjetosVertices);
		}
		void actualizarViewport() {
			glfwSwapBuffers(ventana);
			glfwPollEvents();
		}
		GLFWwindow* getVentana() { return this->ventana; }
		GLfloat getAnchoVentana() { return (float)this->anchoVentana; }
		GLfloat getAltoVentana() { return (float)this->altoVentana; }
		GLfloat getAspecto() { return (float)anchoVentana / (float)altoVentana; }
		GLboolean ventanaDebeCerrarse() {
			if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(ventana, true);
				return true;
			}
			else return false;
		}
		void inicializarBufferVertices(GLuint &id, GLsizeiptr tamBuffer, GLfloat* datos, GLenum tipoDibujo) {
			glGenBuffers(1, &id);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, tamBuffer, datos, tipoDibujo);
		}
		void inicializarBufferIndices(GLuint &id, GLsizeiptr tamBuffer, GLuint* datos, GLenum tipoDibujo) {
			glGenBuffers(1, &id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, tamBuffer, datos, GL_STATIC_DRAW);
		}
		void inicializarTextura(GLuint &id, GLsizei ancho, GLsizei alto, short nroCanales, const void* imgTextura) {
			GLenum formato;
			switch (nroCanales) {
			case 1: formato = GL_RED; break;
			case 3: formato = GL_RGB; break;
			case 4: formato = GL_RGBA; break;
			}
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, formato, ancho, alto, 0, formato, GL_UNSIGNED_BYTE, imgTextura);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		void anexarBuffers(GLuint idBufferVertices, GLuint idBufferIndices) {
			glBindVertexArray(this->idArregloDeObjetosVertices);
			glBindBuffer(GL_ARRAY_BUFFER, idBufferVertices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idBufferIndices);
		}
		void anexarTextura(GLuint id) {
			glActiveTexture(GL_TEXTURE0 + id - 1);//Si el id es 1 => GL_TEXTURE0 + 1 - 1 == GL_TEXTURE0
			glBindTexture(GL_TEXTURE_2D, id);
		}
		void renderizar(GLsizei nVertices, GLboolean indices) {
			if (!indices) glDrawArrays(GL_TRIANGLES, 0, nVertices);
			else glDrawElements(GL_TRIANGLES, nVertices, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		void activarAtributo(GLuint localizacion, GLint cantidad, GLsizei espacioAtributos, const void* inicioAtributo) {
			glVertexAttribPointer(localizacion, cantidad, GL_FLOAT, GL_FALSE, espacioAtributos, inicioAtributo);
			glEnableVertexAttribArray(localizacion);
		}
	};
	class CModelado {
		typedef unordered_map<string, void*> uniform;
		string nombre;
		vec3* posicion;
	public:
		InfoMaterial* infoMaterial;
		CProgramaShaders* progShaders;
		uniform variablesUniformes;
		vector<CTranfGeometrica*> transformaciones;
		CModelado(string nombre) {
			this->nombre = nombre;
			this->progShaders = nullptr;
			this->posicion = new vec3;
		}
		~CModelado() {
			if (this->progShaders) this->progShaders;
			for (CTranfGeometrica* tG : transformaciones) delete tG;
			delete this->posicion;
		}
		void enviarDatosShader(mat4 &vista, mat4 &proyeccion) {
			this->progShaders->usar();
			this->setMVP(vista, proyeccion);
			this->asignarVariablesUnifromes();
		}
		vec3* getPosicion() {
			this->calcularModelo();
			return this->posicion;
		}
		void* getUniform(string nombre) {
			return this->variablesUniformes[nombre];
		}
		void setInfoMaterial(InfoMaterial* infoMaterial) {
			this->infoMaterial = infoMaterial;
		}
	private:
		void setMVP(mat4 &vista, mat4 &proyeccion) {
			this->progShaders->usar();
			this->progShaders->setMat4("modelo", this->calcularModelo());
			this->progShaders->setMat4("vista", vista);
			this->progShaders->setMat4("proyeccion", proyeccion);
		}
		void asignarVariablesUnifromes() {
			for (auto elemento : variablesUniformes) {
				string nombre = (string)elemento.first;
				if (nombre == "color")  this->progShaders->setVec3("color", *((vec3*)elemento.second));
			}
		}
		mat4 calcularModelo() {
			mat4 modelo = mat4(1);
			for (CTranfGeometrica* tG : this->transformaciones)
				modelo = tG->transformar(modelo);
			this->posicion->x = modelo[3].x;
			this->posicion->y = modelo[3].y;
			this->posicion->z = modelo[3].z;
			return modelo;
		}
	};
	class CGrafico {
	protected:
		string nombre, nombreArchivo, formato;
	public:
		InfoLuz* infoLuz;
		CGrafico(string nombre, const string nombreArchivo, string formato) {
			this->nombre = nombre;
			this->nombreArchivo = nombreArchivo.c_str();
			this->formato = formato;
			this->infoLuz = nullptr;
		}
		~CGrafico() {
			if (this->infoLuz) delete infoLuz;
		}
		vector<CModelado*> modelados;
		virtual void dibujar(CRenderizador* renderizador, mat4 vista, mat4 proyeccion) {}
		string getFormato() { return this->formato; }
		// inicializarTextura
	};
	class CGraficoOOF : public CGrafico {
	protected:
		typedef struct { string nombre; short cantidad; } datosAtributo;
		typedef pair<short, datosAtributo> atributo;
		typedef unordered_map<short, datosAtributo> dicAtributos;
		unsigned int nVertices, nTriangulos;
		short numPorVertice;
		dicAtributos atributos;
		GLuint idBufferVertices, idBufferIndices, idTextura, idTexturaEspecular;
		GLfloat* vertices;
		GLuint* indices;
	public:
		CGraficoOOF(string nombre, const string nombreArchivo)
			: CGrafico(nombre, nombreArchivo, "OFF") {
			this->numPorVertice = 0;
		}
		~CGraficoOOF() {
			delete this->vertices, this->indices;
			glDeleteBuffers(1, &this->idBufferVertices);
			if (this->nTriangulos)
				glDeleteBuffers(1, &this->idBufferIndices);
		}
		void dibujar(CRenderizador* renderizador, mat4 vista, mat4 proyeccion) {
			for (CModelado* modelado : modelados) {
				modelado->enviarDatosShader(vista, proyeccion);
				renderizador->anexarBuffers(this->idBufferVertices, this->idBufferIndices);
				short delta = 0;
				for (short localizacion = 0; localizacion < this->atributos.size(); ++localizacion) {
					short cantidad = this->atributos[localizacion].cantidad;
					short espacioAtributos = this->numPorVertice * sizeof(float);
					const void* inicio = (void*)(delta * sizeof(float));
					renderizador->activarAtributo(localizacion, cantidad, espacioAtributos, inicio);
					delta += cantidad;
				}
				int nVertices = (this->nTriangulos == 0) ? this->nVertices : this->nTriangulos * 3;
				renderizador->renderizar(nVertices, this->nTriangulos);
			}
		}
		void setAtributo(short localizacion, string nombre, short cantidad) {
			atributos[localizacion].nombre = nombre;
			atributos[localizacion].cantidad = cantidad;
			this->numPorVertice += cantidad;
		}
		void inicializar(CRenderizador* renderizador, string archivoTextura = "", string archivoTexturaEspecular = "") {
			//TODO: Sólo los gráficos que no emiten luz tendrán textura especular, pasar la textura especular a la clase hija
			this->cargarSuperficie(renderizador);
			this->inicializarBuffers(renderizador);
			this->inicializarTextura(renderizador, archivoTextura, "material.difuso", this->idTextura);
			this->inicializarTextura(renderizador, archivoTexturaEspecular, "material.especular", this->idTexturaEspecular);
		}
	protected:
		void cargarSuperficie(CRenderizador* renderizador) {
			ifstream archivo(nombreArchivo, ifstream::in);
			if (!archivo) {
				cout << "La superficie no fue cargada correctamente" << endl;
				return;
			}
			string formato;
			getline(archivo, formato);
			if (formato != "OFF")
			{
				cout << "El archivo no se encuentra en formato OFF." << endl;
				return;
			}
			float aux;
			archivo >> this->nVertices >> this->nTriangulos >> aux;
			vertices = new GLfloat[this->nVertices   * this->numPorVertice];
			indices = new GLuint[this->nTriangulos * 3];
			for (unsigned int i = 0, k = 0; i < this->nVertices; ++i, k = 0) {
				for (atributo datos : this->atributos) {
					for (unsigned short j = 0; j < datos.second.cantidad; ++j) {
						float val;
						archivo >> val;
						this->vertices[this->numPorVertice * i + k++] = val;
					}
				}
			}
			for (unsigned int i = 0; i < this->nTriangulos; ++i)
				archivo >> aux >> this->indices[3 * i]
				>> this->indices[3 * i + 1]
				>> this->indices[3 * i + 2];
			archivo.close();
		}
		void inicializarBuffers(CRenderizador* renderizador) {
			int tamBuffer;
			tamBuffer = this->nVertices * this->numPorVertice * sizeof(GLfloat);
			renderizador->inicializarBufferVertices(this->idBufferVertices, tamBuffer, this->vertices, GL_STATIC_DRAW);
			if (this->nTriangulos) {
				tamBuffer = this->nTriangulos * 3 * sizeof(GLuint);
				renderizador->inicializarBufferIndices(this->idBufferIndices, tamBuffer, this->indices, GL_STATIC_DRAW);
			}
		}
		void inicializarTextura(CRenderizador* renderizador, string archivoTextura, string material_tipo, GLuint &id) {
			if (archivoTextura != "") {
				int ancho, alto, nroCanales;
				stbi_set_flip_vertically_on_load(true);
				unsigned char *imgTextura = stbi_load(archivoTextura.c_str(), &ancho, &alto, &nroCanales, 0);
				if (imgTextura) renderizador->inicializarTextura(id, ancho, alto, nroCanales, imgTextura);
				else cout << "ERROR: No se pudo cargar la textura." << std::endl;
				stbi_image_free(imgTextura);
			}
		}
	};
	class CGraficoObjeto : public CGraficoOOF {
	public:
		CGraficoObjeto(string nombre, string nombreArchivo)
			: CGraficoOOF(nombre, nombreArchivo) {}
		void dibujar(CRenderizador* renderizador, mat4 &vista, mat4 &proyeccion, vec3 posVista, vector<CGrafico*> &graficosLuz) {
			for (CModelado* modelado : modelados) {
				modelado->enviarDatosShader(vista, proyeccion);
				modelado->progShaders->setVec3("posVista", posVista);
				this->setInfoLuces(modelado, graficosLuz);
				renderizador->anexarBuffers(this->idBufferVertices, this->idBufferIndices);
				short delta = 0;
				for (short localizacion = 0; localizacion < this->atributos.size(); ++localizacion) {
					short cantidad = this->atributos[localizacion].cantidad;
					short espacioAtributos = this->numPorVertice * sizeof(float);
					const void* inicio = (void*)(delta * sizeof(float));
					renderizador->activarAtributo(localizacion, cantidad, espacioAtributos, inicio);
					delta += cantidad;
				}
				if (this->idTextura > 0) {
					modelado->progShaders->setInt("material.difuso", 0);
					modelado->progShaders->setInt("material.especular", 1);
					this->setInfoTextura(modelado);
					renderizador->anexarTextura(idTextura);
					renderizador->anexarTextura(idTexturaEspecular);
				}
				else this->setInfoMaterial(modelado);
				int nVertices = (this->nTriangulos == 0) ? this->nVertices : this->nTriangulos * 3;
				renderizador->renderizar(nVertices, this->nTriangulos);
			}
		}
	private:
		void setInfoLuces(CModelado* modelado, vector<CGrafico*> &graficosLuz) {
			for (CGrafico* graficoLuz : graficosLuz) {
				InfoLuz* infoLuz = graficoLuz->infoLuz;
				//Estamos chancando siempre la última infoLuz de cada tipo
				if (infoLuz->tipo == "Direccionada") {
					modelado->progShaders->setVec3("luzDireccionada.direccion"      , ((InfoLuzDireccionada*)infoLuz)->direccion      );
					modelado->progShaders->setVec3("luzDireccionada.color.ambiente" , ((InfoLuzDireccionada*)infoLuz)->color.ambiente );
					modelado->progShaders->setVec3("luzDireccionada.color.especular", ((InfoLuzDireccionada*)infoLuz)->color.especular);
					modelado->progShaders->setVec3("luzDireccionada.color.difuso"   , ((InfoLuzDireccionada*)infoLuz)->color.difuso   );
				}
				else if (infoLuz->tipo == "Puntual") {
					modelado->progShaders->setVec3("luzPuntual.posicion"                   , ((InfoLuzPuntual*)infoLuz)->posicion                        );
					modelado->progShaders->setVec3("luzPuntual.color.ambiente"             , ((InfoLuzPuntual*)infoLuz)->color.ambiente                  );
					modelado->progShaders->setVec3("luzPuntual.color.especular"            , ((InfoLuzPuntual*)infoLuz)->color.especular                 );
					modelado->progShaders->setVec3("luzPuntual.color.difuso"               , ((InfoLuzPuntual*)infoLuz)->color.difuso                    );
					modelado->progShaders->setFloat("luzPuntual.constAtenuacion.constante" , ((InfoLuzPuntual*)infoLuz)->constantesAtenuacion->constante );
					modelado->progShaders->setFloat("luzPuntual.constAtenuacion.linear"    , ((InfoLuzPuntual*)infoLuz)->constantesAtenuacion->linear    );
					modelado->progShaders->setFloat("luzPuntual.constAtenuacion.cuadratica", ((InfoLuzPuntual*)infoLuz)->constantesAtenuacion->cuadratica);
				}
				else if (infoLuz->tipo == "Focal") {
					modelado->progShaders->setVec3("luzFocal.posicion"                   , ((InfoLuzFocal*)infoLuz)->posicion                        );
					modelado->progShaders->setVec3("luzFocal.direccion"                  , ((InfoLuzFocal*)infoLuz)->direccion                       );
					modelado->progShaders->setVec3("luzFocal.color.ambiente"             , ((InfoLuzFocal*)infoLuz)->color.ambiente                  );
					modelado->progShaders->setVec3("luzFocal.color.especular"            , ((InfoLuzFocal*)infoLuz)->color.especular                 );
					modelado->progShaders->setVec3("luzFocal.color.difuso"               , ((InfoLuzFocal*)infoLuz)->color.difuso                    );
					modelado->progShaders->setFloat("luzFocal.constAtenuacion.constante" , ((InfoLuzFocal*)infoLuz)->constantesAtenuacion->constante );
					modelado->progShaders->setFloat("luzFocal.constAtenuacion.linear"    , ((InfoLuzFocal*)infoLuz)->constantesAtenuacion->linear    );
					modelado->progShaders->setFloat("luzFocal.constAtenuacion.cuadratica", ((InfoLuzFocal*)infoLuz)->constantesAtenuacion->cuadratica);
					modelado->progShaders->setFloat("luzFocal.anguloInterno"             , ((InfoLuzFocal*)infoLuz)->anguloInterno                   );
					modelado->progShaders->setFloat("luzFocal.anguloExterno"             , ((InfoLuzFocal*)infoLuz)->anguloExterno                   );
				}
			}
		}
		void setInfoMaterial(CModelado* modelado) {
			modelado->progShaders->setVec3("material.ambiente", modelado->infoMaterial->ambiente);
			modelado->progShaders->setVec3("material.especular", modelado->infoMaterial->especular);
			modelado->progShaders->setVec3("material.difuso", modelado->infoMaterial->difuso);
			modelado->progShaders->setFloat("material.brillo", modelado->infoMaterial->brillo);
		}
		void setInfoTextura(CModelado* modelado) {
			modelado->progShaders->setFloat("material.brillo", 32.0f);
		}
	};
	class CGraficoOBJ : public CGrafico
	{
		class CSuperficieOBJ {
			struct Vertice {
				vec3 posicion;
				vec3 normal;
				vec2 coordTextura;
				vec3 tangente;
				vec3 bitangente;
			};
			struct Textura {
				unsigned int id;
				string tipo;
				string ruta;
			};
			unsigned int idBufferVertices, idBufferIndices;
		public:
			vector<Vertice>      vertices;
			vector<unsigned int> indices;
			vector<Textura>      texturas;
			unsigned int idArregloDeObjetosVertices;
			CSuperficieOBJ(const aiScene* aiModelo, aiMesh* aiSuperficie, string directorio)
			{
				procesarAiSuperficie(aiModelo, aiSuperficie, directorio);
				inicializarBuffersAtributos();
			}
			void dibujar()
			{
				unsigned int iAmbiente, iDifuso, iEspecular, iNormal, i;
				iAmbiente = iDifuso = iEspecular = iNormal = i = 0;
				for (Textura textura : texturas)
				{
					glActiveTexture(GL_TEXTURE0 + i++);
					string num;
					if (textura.tipo == "textura_ambiente") num = to_string(++iAmbiente);
					else if (textura.tipo == "textura_difusa") num = to_string(++iDifuso);
					else if (textura.tipo == "textura_especular") num = to_string(++iEspecular);
					else if (textura.tipo == "textura_normal") num = to_string(++iNormal);
					glBindTexture(GL_TEXTURE_2D, textura.id);
				}
				glBindVertexArray(idArregloDeObjetosVertices);
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		private:
			void procesarAiSuperficie(const aiScene* aiModelo, aiMesh* aiSuperficie, string directorio) {
				// Definiendo lambdas para facilitar el proceso de copia de aiVector3D a vectores glm
				auto copiarVector2D = [](vec2 &vector, aiVector3D &vec_assimp, bool existe) {
					vector.x = existe ? vec_assimp.x : 0;
					vector.y = existe ? vec_assimp.y : 0;
				};
				auto copiarVector3D = [](vec3 &vector, aiVector3D &vec_assimp) {
					vector.x = vec_assimp.x;
					vector.y = vec_assimp.y;
					vector.z = vec_assimp.z;
				};
				// Copiando los vértices: posiciones, normales, coordenadas de texturas, tangentes y bitangentes por cada vértice
				for (unsigned int i = 0; i < aiSuperficie->mNumVertices; ++i)
				{
					Vertice vertex;
					copiarVector3D(vertex.posicion, aiSuperficie->mVertices[i]);
					copiarVector3D(vertex.normal, aiSuperficie->mNormals[i]);
					copiarVector2D(vertex.coordTextura, aiSuperficie->mTextureCoords[0][i], aiSuperficie->mTextureCoords[0]);
					copiarVector3D(vertex.tangente, aiSuperficie->mTangents[i]);
					copiarVector3D(vertex.bitangente, aiSuperficie->mBitangents[i]);
					this->vertices.push_back(vertex);
				}
				// Copiando las caras: índices
				for (unsigned int i = 0; i < aiSuperficie->mNumFaces; ++i)
				{
					aiFace cara = aiSuperficie->mFaces[i];
					for (unsigned int j = 0; j < cara.mNumIndices; ++j)
						this->indices.push_back(cara.mIndices[j]);
				}
				// Copiando (cargando) las texturas: normales, ambiente, difusa, especular
				aiMaterial* material = aiModelo->mMaterials[aiSuperficie->mMaterialIndex];
				auto agregarTexturas = [&](aiTextureType tipoTextura, string nombreTipoTextura) {
					vector<Textura> texturaEspecifica = cargarTexturasDeMateriales(material, tipoTextura, nombreTipoTextura, directorio);
					this->texturas.insert(texturas.end(), texturaEspecifica.begin(), texturaEspecifica.end());
				};
				agregarTexturas(aiTextureType_HEIGHT, "textura_normal");
				agregarTexturas(aiTextureType_AMBIENT, "textura_ambiente");
				agregarTexturas(aiTextureType_DIFFUSE, "textura_difusa");
				agregarTexturas(aiTextureType_SPECULAR, "textura_especular");
			}
			vector<Textura> cargarTexturasDeMateriales(aiMaterial *mat, aiTextureType type, string typeName, string directorio)
			{
				vector<Textura> texturas;
				for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
				{
					aiString nombreArchivo;
					mat->GetTexture(type, i, &nombreArchivo);
					bool fueInicializada = false;
					for (Textura textura : texturas)
					{
						if (textura.ruta == nombreArchivo.C_Str())
						{
							texturas.push_back(textura);
							fueInicializada = true;
							break;
						}
					}
					if (!fueInicializada)
					{
						Textura textura;
						textura.id = inicializarTextura(directorio + '/' + nombreArchivo.C_Str());
						textura.tipo = typeName;
						textura.ruta = nombreArchivo.C_Str();
						texturas.push_back(textura);
						texturas.push_back(textura);
					}
				}
				return texturas;
			}
			//TODO: Adaptar la función para derivar la mayor parte a CRenderizador::inicializarTextura
			unsigned int inicializarTextura(string nombreArchivo, bool gamma = false)
			{
				unsigned int id;
				glGenTextures(1, &id);

				int ancho, alto, nroCanales;
				unsigned char *imgTextura = stbi_load(nombreArchivo.c_str(), &ancho, &alto, &nroCanales, 0);
				if (imgTextura)
				{
					GLenum formato;
					switch (nroCanales) {
					case 1: formato = GL_RED; break;
					case 3: formato = GL_RGB; break;
					case 4: formato = GL_RGBA; break;
					}

					glBindTexture(GL_TEXTURE_2D, id);
					glTexImage2D(GL_TEXTURE_2D, 0, formato, ancho, alto, 0, formato, GL_UNSIGNED_BYTE, imgTextura);
					glGenerateMipmap(GL_TEXTURE_2D);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					stbi_image_free(imgTextura);
				}
				else
				{
					cout << "Error al cargar la textura: " << nombreArchivo << endl;
					stbi_image_free(imgTextura);
				}

				return id;
			}
			void inicializarBuffersAtributos()
			{
				glGenVertexArrays(1, &idArregloDeObjetosVertices);
				glGenBuffers(1, &idBufferVertices);
				glGenBuffers(1, &idBufferIndices);

				glBindVertexArray(idArregloDeObjetosVertices);

				glBindBuffer(GL_ARRAY_BUFFER, idBufferVertices);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertice), &vertices[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idBufferIndices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, normal));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, coordTextura));
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, tangente));
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, bitangente));

				glBindVertexArray(0);
			}
		};
		vector<CSuperficieOBJ> superficies;
		bool correccionGama;
	public:
		CGraficoOBJ(string nombre, const string &nombreArchivo, bool correccionGama = false) 
			: CGrafico(nombre, nombreArchivo, "OBJ")
		{
			this->correccionGama = correccionGama;
			this->cargarModelo(nombreArchivo);
		}
		void dibujar(CRenderizador* renderizador, mat4 vista, mat4 proyeccion)//TODO: enviar renderizador a CSuperficie para librarla de renderizar
		{
			for (CModelado* modelado : this->modelados) {
				modelado->enviarDatosShader(vista, proyeccion);
				for (unsigned int i = 0; i < superficies.size(); i++)
					superficies[i].dibujar();
			}
		}
	private:
		void cargarModelo(const string &ruta)
		{
			Assimp::Importer importador;
			//Opciones de Postprocesamiento: http://assimp.sourceforge.net/lib_html/postprocess_8h.html
			const aiScene* modelo = importador.ReadFile(ruta, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);
			// verificando si hubieron errores
			if (!modelo || modelo->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelo->mRootNode)
			{
				cout << "ERROR::ASSIMP:: " << importador.GetErrorString() << endl;
				return;
			}
			procesarNodo(modelo->mRootNode, modelo, ruta.substr(0, ruta.find_last_of('/')));
		}
		void procesarNodo(aiNode* nodo, const aiScene* aiModelo, string directorio)
		{
			//Procesamos cada superficie relacionada al nodo actual
			for (unsigned int i = 0; i < nodo->mNumMeshes; i++)
			{
				unsigned int idSuperficieNodo = nodo->mMeshes[i];
				aiMesh* aiSuperficie = aiModelo->mMeshes[idSuperficieNodo];
				superficies.push_back(CSuperficieOBJ(aiModelo, aiSuperficie, directorio));
			}
			//Continuamos procesando los subnodos del nodo actual
			for (unsigned int i = 0; i < nodo->mNumChildren; i++)
				procesarNodo(nodo->mChildren[i], aiModelo, directorio);
		}
	};

	//class CGraficoObjetoColor
	//class CGraficoObjetoTextura
	class CCamara
	{
		// Sistema de Coordenadas
		vec3 posicionCamara;
		vec3 arribaMundo;
		vec3 frenteCamara;
		vec3 arribaCamara;
		vec3 derechaCamara;
		// Ángulos de Euler
		float yaw, pitch;
		bool restriccionPitch;
		// Opciones de Cámara
		float velocidadMov, sensibilidadGiro;
		float zoom;
	public:
		CCamara(vec3 posicion = vec3(0, 0, 3), vec3 arriba = vec3(0, 1, 0), float yaw = -90, float pitch = 0) {
			this->posicionCamara = posicion;
			this->arribaMundo = arriba;
			this->yaw = yaw;
			this->pitch = pitch;
			this->frenteCamara = vec3(0, 0, -1);
			this->velocidadMov = 2.5;
			this->sensibilidadGiro = 0.1f;
			this->zoom = 45.f;
			this->restriccionPitch = true;
			this->actualizarSistemaCoordenadas();
		}
		mat4 getMatrizVista() {
			return lookAt(posicionCamara, posicionCamara + frenteCamara, arribaCamara);
		}
		mat4 getMatrizPerspectiva(float aspecto = 800. / 600.) {
			return perspective(radians(zoom), aspecto, 0.1f, 100.f);
		}
		void mover(MovimientoCamara direccion, float deltaTiempo) {
			float velocidad = this->velocidadMov * deltaTiempo;
			switch (direccion)
			{
			case ADELANTE: this->posicionCamara += this->frenteCamara  * velocidad; break;
			case ATRAS: this->posicionCamara -= this->frenteCamara  * velocidad; break;
			case IZQUIERDA: this->posicionCamara -= this->derechaCamara * velocidad; break;
			case DERECHA: this->posicionCamara += this->derechaCamara * velocidad; break;
			}
		}
		void girar(float dYaw, float dPitch) {
			dYaw *= this->sensibilidadGiro;
			dPitch *= this->sensibilidadGiro;
			this->yaw += dYaw;
			this->pitch += dPitch;
			if (this->restriccionPitch)
			{
				if (this->pitch > 89.0f) this->pitch = 89.f;
				if (this->pitch < -89.0f) this->pitch = -89.f;
			}
			this->actualizarSistemaCoordenadas();
		}
		void acercar(float dZoom) {
			if (this->zoom >= 1.0f && this->zoom <= 45.0f) this->zoom -= dZoom;
			if (this->zoom <= 1.0f)                        this->zoom = 1.f;
			if (this->zoom >= 45.0f)                        this->zoom = 45.f;
		}
		vec3 getPosicion() {
			return this->posicionCamara;
		}
		vec3 getFrente() {
			return this->frenteCamara;
		}
	private:
		void actualizarSistemaCoordenadas() {
			this->frenteCamara.x = cos(radians(this->pitch)) * cos(radians(this->yaw));
			this->frenteCamara.y = sin(radians(this->pitch));
			this->frenteCamara.z = cos(radians(this->pitch)) * sin(radians(this->yaw));
			//Recalculamos el sistema de coordenadas
			this->frenteCamara = normalize(this->frenteCamara);
			this->derechaCamara = normalize(cross(this->frenteCamara, this->arribaMundo));
			this->arribaCamara = normalize(cross(this->derechaCamara, this->frenteCamara));
		}
	};
	class CControladora {
		vector<CGrafico*> graficosLuz;
		vector<CGrafico*> graficosObjeto;//TODO: agrupar todos los objetos que no son de Luz en CGraficoObjeto
		CRenderizador* renderizador;
		CCamara* camara;
		CInfoConstantes* infoConstantes;
		float tiempoFrameAnterior, difTiempoFrames;
	public:
		CControladora(unsigned int anchoPantalla = 800, unsigned int altoPantalla = 600) {
			this->renderizador = new CRenderizador(anchoPantalla, altoPantalla);
			this->camara = new CCamara();
			this->infoConstantes = new CInfoConstantes();
			this->difTiempoFrames = 0;
			this->tiempoFrameAnterior = 0;
		}
		~CControladora() {
			for (CGrafico* luz : this->graficosLuz) delete luz;
			for (CGrafico* objeto : this->graficosObjeto) delete objeto;
			delete this->renderizador;
			delete this->camara;
			delete this->infoConstantes;
		}
		void inicializar(const char* titulo, const char* nombreArchivo) {
			if (!this->renderizador->inicializar(titulo)) return;
			if (!this->cargarGraficosYModelados(this->renderizador, nombreArchivo)) return;
			this->animarGraficos();
		}
		void animarGraficos() {
			while (!this->renderizador->ventanaDebeCerrarse()) {
				desplazarCamara(this->renderizador->getVentana());
				this->actualizarTiempoEntreFrames();
				this->renderizador->refrescarViewport();
				this->renderizarGraficos(this->renderizador, this->camara->getMatrizVista(),
					this->camara->getMatrizPerspectiva(this->renderizador->getAspecto()));
				this->renderizador->actualizarViewport();
			}
		}
		float getAnchoVentana() { return this->renderizador->getAnchoVentana(); }
		float getAltoVentana() { return this->renderizador->getAltoVentana(); }
		float getTiempoEntreFrames() { return this->difTiempoFrames; }
		CCamara* getCamara() { return this->camara; }
		CRenderizador* getRenderizador() { return this->renderizador; }
	private:
		bool cargarGraficosYModelados(CRenderizador* renderizador, const char* nombreArchivo) {
			//TODO: Esta función creció demasiado, tiene potencial para ser una clase (código mejor distribuido y lectura mas clara)
			ifstream pArchivo(nombreArchivo, ifstream::in);
			if (!pArchivo) {
				cout << "El archivo de objetos gráficos no pudo ser leído." << endl;
				return false;
			}
			stringstream streamArchivo;
			streamArchivo << pArchivo.rdbuf();
			CharReaderBuilder lector;
			Value raiz;
			String errores;
			parseFromStream(lector, streamArchivo, &raiz, &errores);
			const Value& objetos = raiz["Objetos"];
			for (const Value& objeto : objetos) {
				CGrafico* grafico = nullptr;
				string nombreObjeto, archivoGrafico, archivoTextura, archivoTexturaEspecular, tipo;
				nombreObjeto = objeto["Nombre"].asString();
				archivoGrafico = objeto["Archivo"].asString();
				bool luz = objeto["Luz"].asBool();

				vector<CGrafico*>* graficos = &this->graficosObjeto;
				InfoLuz* infoLuz = nullptr;
				if (luz) {
					graficos = &this->graficosLuz;
					//TODO: generalizar también para graficos OBJ
					tipo = objeto["Tipo"].asString();
					const Value& detalleLuz = objeto["DetalleLuz"];
					if (tipo == "Direccionada") {
						InfoLuzDireccionada* infoLuzDireccionada = new InfoLuzDireccionada();
						infoLuzDireccionada->direccion.x = detalleLuz["Direccion"]["x"].asFloat();
						infoLuzDireccionada->direccion.y = detalleLuz["Direccion"]["y"].asFloat();
						infoLuzDireccionada->direccion.z = detalleLuz["Direccion"]["z"].asFloat();
						infoLuz = (InfoLuz*)infoLuzDireccionada;
					}
					else if (tipo == "Puntual") {
						InfoLuzPuntual* infoLuzPuntual = new InfoLuzPuntual();
						short distancia = (short)detalleLuz["DistAtenuacion"].asInt();
						infoLuzPuntual->constantesAtenuacion = this->infoConstantes->getInfoConstAtenuacion(distancia);
						infoLuz = (InfoLuz*)infoLuzPuntual;
					}
					else {//if (tipo == "Focal")
						InfoLuzFocal* infoLuzFocal = new InfoLuzFocal();
						short distancia = (short)detalleLuz["DistAtenuacion"].asInt();
						infoLuzFocal->constantesAtenuacion = this->infoConstantes->getInfoConstAtenuacion(distancia);
						infoLuzFocal->anguloInterno = cos(radians(detalleLuz["AnguloInterno"].asFloat()));
						infoLuzFocal->anguloExterno = cos(radians(detalleLuz["AnguloExterno"].asFloat()));
						infoLuz = (InfoLuz*)infoLuzFocal;
					}
					infoLuz->color.ambiente.r = detalleLuz["Color"]["Ambiente"]["r"].asFloat();
					infoLuz->color.ambiente.g = detalleLuz["Color"]["Ambiente"]["g"].asFloat();
					infoLuz->color.ambiente.b = detalleLuz["Color"]["Ambiente"]["b"].asFloat();
					infoLuz->color.especular.r = detalleLuz["Color"]["Especular"]["r"].asFloat();
					infoLuz->color.especular.g = detalleLuz["Color"]["Especular"]["g"].asFloat();
					infoLuz->color.especular.b = detalleLuz["Color"]["Especular"]["b"].asFloat();
					infoLuz->color.difuso.r = detalleLuz["Color"]["Difuso"]["r"].asFloat();
					infoLuz->color.difuso.g = detalleLuz["Color"]["Difuso"]["g"].asFloat();
					infoLuz->color.difuso.b = detalleLuz["Color"]["Difuso"]["b"].asFloat();
				}
				if (objeto["Formato"].asString() == "OBJ") {
					grafico = new CGraficoOBJ(nombreObjeto, archivoGrafico);
					grafico->infoLuz = infoLuz;
				}
				else {
					archivoTextura = objeto["Textura2D"].asString();
					archivoTexturaEspecular = objeto["Textura2D_Especular"].asString();
					grafico = new CGraficoObjeto(nombreObjeto, archivoGrafico);
					grafico->infoLuz = infoLuz;
					const Value& atributos = objeto["Atributos"];
					for (string localizacion : atributos.getMemberNames()) {
						((CGraficoOOF*)grafico)->setAtributo(stoi(localizacion), atributos[localizacion]["Nombre"].asString(), atributos[localizacion]["Cantidad"].asInt());
					}
					((CGraficoOOF*)grafico)->inicializar(renderizador, archivoTextura, archivoTexturaEspecular);
				}
				graficos->push_back(grafico);
				const Value& modelos = objeto["Modelos"];
				for (const Value& modelo : modelos) {
					CModelado* modelado = new CModelado(nombreObjeto);
					string fShaderDeVertice = modelo["Shader de Vertice"].asString();
					string fShaderDeFragmento = modelo["Shader de Fragmento"].asString();
					modelado->progShaders = new CProgramaShaders(fShaderDeVertice, fShaderDeFragmento);
					if (grafico->getFormato() == "OFF") {
						string nombreMaterial = modelo["Material"].asString();
						modelado->setInfoMaterial(this->infoConstantes->getInfoMaterial(nombreMaterial));
						const Value& uniformes = modelo["Uniformes"];
						for (const Value& uniforme : uniformes) {
							string nombreVarUniforme = uniforme["nombre"].asString();
							if (nombreVarUniforme == "color") {
								float r, g, b;
								r = uniforme["valor"]["r"].asFloat();
								g = uniforme["valor"]["g"].asFloat();
								b = uniforme["valor"]["b"].asFloat();
								modelado->variablesUniformes[nombreVarUniforme] = new vec3(r, g, b);
							}
							//AQUI AGREGAR CóDIGO DE OTRAS VARIABLES UNIFORMES
						}
					}
					const Value& transformaciones = modelo["Transformaciones"];
					for (const Value& transformacion : transformaciones) {
						string tipoTransGeometrica = transformacion["TransGeometrica"].asString();
						float x, y, z, angulo;
						x = transformacion["x"].asFloat();
						y = transformacion["y"].asFloat();
						z = transformacion["z"].asFloat();
						if (tipoTransGeometrica == "Traslacion")
							modelado->transformaciones.push_back(new CTranfGeometrica(Traslacion, vec3(x, y, z)));
						else if (tipoTransGeometrica == "Escalamiento")
							modelado->transformaciones.push_back(new CTranfGeometrica(Escalamiento, vec3(x, y, z)));
						else if (tipoTransGeometrica == "Rotacion") {
							angulo = transformacion["angulo"].asFloat();
							modelado->transformaciones.push_back(new CTranfGeometrica(Rotacion, vec3(x, y, z), angulo));
						}
					}
					grafico->modelados.push_back(modelado);
				}
			}
			return true;
		}
		void renderizarGraficos(CRenderizador* renderizador, mat4 vista, mat4 proyeccion) {
			for (CGrafico* grafico : this->graficosLuz) {
				if (grafico->infoLuz->tipo == "Focal") {
					//Podríamos indicar la posición internamente, como en el caso de la luz puntual
					((InfoLuzFocal*)grafico->infoLuz)->posicion = this->camara->getPosicion();
					((InfoLuzFocal*)grafico->infoLuz)->direccion = this->camara->getFrente();
				}
				grafico->dibujar(renderizador, vista, proyeccion);
			}
			for (CGrafico* grafico : this->graficosObjeto) {
				if(grafico->getFormato() == "OBJ")
					((CGraficoOBJ*)grafico)->dibujar(renderizador, vista, proyeccion);
				else
					((CGraficoObjeto*)grafico)->dibujar(renderizador, vista, proyeccion, this->camara->getPosicion(), this->graficosLuz);
			}
		}
		void actualizarTiempoEntreFrames() {
			float tiempoFrameActual = (float)glfwGetTime();
			this->difTiempoFrames = tiempoFrameActual - this->tiempoFrameAnterior;
			this->tiempoFrameAnterior = tiempoFrameActual;
		}
	};

	CControladora controlador;
	float xAnt = controlador.getAnchoVentana() / 2, yAnt = controlador.getAltoVentana() / 2;
	bool inicio = true;

	void redimensionarBuffer(GLFWwindow* ventana, int ancho, int alto)
	{
		glViewport(0, 0, (GLsizei)controlador.getRenderizador()->getAnchoVentana(), (GLsizei)controlador.getRenderizador()->getAltoVentana());
	}
	void desplazarCamara(GLFWwindow* ventana) {
		float difTiempo = controlador.getTiempoEntreFrames();
		if (glfwGetKey(ventana, GLFW_KEY_W) == GLFW_PRESS) controlador.getCamara()->mover(ADELANTE, difTiempo);
		if (glfwGetKey(ventana, GLFW_KEY_S) == GLFW_PRESS) controlador.getCamara()->mover(ATRAS, difTiempo);
		if (glfwGetKey(ventana, GLFW_KEY_A) == GLFW_PRESS) controlador.getCamara()->mover(IZQUIERDA, difTiempo);
		if (glfwGetKey(ventana, GLFW_KEY_D) == GLFW_PRESS) controlador.getCamara()->mover(DERECHA, difTiempo);
	}
	void girarCamara(GLFWwindow* window, double x, double y)
	{
		if (inicio)
		{
			xAnt = (float)x;
			yAnt = (float)y;
			inicio = false;
		}
		float dYaw = (float)x - xAnt;
		float dPitch = yAnt - (float)y; // en reversa pues las coordenadas van de abajo hacia arriba
		controlador.getCamara()->girar(dYaw, dPitch);
		xAnt = (float)x;
		yAnt = (float)y;
	}
	void acercarCamara(GLFWwindow* window, double dx, double dZoom)
	{
		controlador.getCamara()->acercar(float(dZoom));
	}
}
#endif