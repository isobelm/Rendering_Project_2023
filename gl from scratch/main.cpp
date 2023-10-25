#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

//#define STB_IMAGE_IMPLEMENTATION
//#include "Libraries/stb_image.h"
#include "stbInclude.h"

#include "Window.hpp"
#include "Image.hpp"
#include "Shader.hpp"
#include "Particle.hpp"
#include "ParticleSystem.hpp"

using namespace std;

void initGlfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

int stage = 1;
int blendMode = 0, numBlendModes = 4;
bool change = false;
Window* window;
ParticleSystem *particleSystem;
Image* image;
float clearColour = 0.0f;;

void clearScreen() {
	glClearColor(clearColour, clearColour, clearColour, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window->window);
	glClearColor(clearColour, clearColour, clearColour, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window->window);
}


void setBlendMode(int mode) {
	switch(mode) {
		case 0:
		default:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			break;
		case 1:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			clearColour = 0.0f;
			break;
		case 2:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			clearColour = 1.0f;
			break;
		case 3:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glBlendEquation(GL_MAX);
			clearColour = 0.0f;
			break;
	}
}

void drawLoop(Shader* shader, Shader* particleShader, ParticleSystem* particleSystem) {
	if (window->window == NULL) return;
	
	clearScreen();
	setBlendMode(blendMode);
	
	double lastTime = glfwGetTime();
	int frameCount = 0;
	
	while (!glfwWindowShouldClose(window->window) and !change) {
		window->processInput();
		particleSystem->updateParticles();
		
		glClear(GL_DEPTH_BUFFER_BIT);
		
		particleSystem->drawParticles();
		
		glfwSwapBuffers(window->window);
		frameCount++;
		double currentTime = glfwGetTime();
		frameCount++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%d FPS\n", frameCount);
			
			frameCount = 0;
			lastTime += 1.0;
		}
		glfwPollEvents();
	}
}

char* filePath = NULL;
void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	if (count > 0) {
		filePath = (char*)malloc(sizeof(const char) * std::strlen(*paths));
		std::strcpy(filePath, *paths);
	}
}


void dragAndDropLoop() {
	filePath = NULL;

	clearScreen();
	setBlendMode(0);
	
	while (filePath == NULL and !glfwWindowShouldClose(window->window))
	{
		window->processInput();
		glfwPollEvents();
	}
}

void dragAndDropLoopBrush() {
	filePath = NULL;

	clearScreen();
	image->draw();
	glfwSwapBuffers(window->window);
	
	while (filePath == NULL and !glfwWindowShouldClose(window->window))
	{
		window->processInput();
		glfwPollEvents();
	}

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (stage == 3) {
		float newSize = particleSystem->size + yoffset;
		if (newSize < 0) {
			newSize = 0.1;
		}
		particleSystem->size = newSize;
	}
}

Image* getImage(Shader* shader) {
	dragAndDropLoop();
	if (glfwWindowShouldClose(window->window)) return NULL;
	
	image = new Image(filePath, shader);
	image->bindBuffers();
	window->setSize(image->width, image->height);
	
	stage++;
	return image;
}

ParticleSystem* getParticles(Shader* shader) {
	glm::mat4 projection = glm::ortho(0.0f, (float)image->width, (float)image->height, 0.0f, -1.0f, 1.0f);
	
	dragAndDropLoopBrush();
	if (glfwWindowShouldClose(window->window)) return NULL;
	
	particleSystem = new ParticleSystem(shader, projection, filePath, image);
	glfwSetScrollCallback(window->window, scrollCallback);
	
	stage++;
	change = false;
	return  particleSystem;
}

int loop(Shader* shader, Shader* particleShader) {
	
	while (!glfwWindowShouldClose(window->window)) {
		if (stage == 3) drawLoop(shader, particleShader, particleSystem);
		else if (stage == 1) image = getImage(shader);
		else  getParticles(particleShader);
	}
	
//	image->freeData();
	glfwTerminate();
	return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		delete image;
		image = NULL;
		delete particleSystem;
		particleSystem = NULL;
		change = true;
		stage = 1;
		clearScreen();
	}
}


void changeBlendMode(int change) {
	blendMode = (blendMode + change) % numBlendModes;
	if (blendMode < 0) blendMode = numBlendModes - 1;
	setBlendMode(blendMode);
	clearScreen();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (stage > 1) {
		if (key == GLFW_KEY_UP and action == GLFW_RELEASE) image->yBias--;
		if (key == GLFW_KEY_DOWN and action == GLFW_RELEASE) image->yBias++;
		if (key == GLFW_KEY_RIGHT and action == GLFW_RELEASE) image->xBias++;
		if (key == GLFW_KEY_LEFT and action == GLFW_RELEASE) image->xBias--;
		}
	if (stage == 3) {
		if (key == GLFW_KEY_Z and action == GLFW_RELEASE) {
			clearColour = clearColour + 0.1f;
			if (clearColour > 1.0f) clearColour = 1.0f;
			clearScreen();
		}
		if (key == GLFW_KEY_X and action == GLFW_RELEASE) {
			clearColour = clearColour - 0.1f;
			if (clearColour < 0) clearColour = 0;
			clearScreen();
			}
		if (key == GLFW_KEY_W and action == GLFW_RELEASE) particleSystem->speed *= 1.25;
		if (key == GLFW_KEY_S and action == GLFW_RELEASE) particleSystem->speed /= 1.25;
		
		if (key == GLFW_KEY_D and action == GLFW_RELEASE) changeBlendMode(1);
		if (key == GLFW_KEY_A and action == GLFW_RELEASE) changeBlendMode(-1);
		

	}
}

int main() {
	initGlfw();
	window = new Window(1200, 1000);
	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
//	glBlendEquationSeparate(GL_FUNC_ADD, )
	

	Shader* imageShader = new Shader("Shaders/vs.glsl", "Shaders/fs.glsl");
	Shader* particleShader = new Shader("Shaders/particle_vs.glsl", "Shaders/particle_fs.glsl");
	
	glfwSetDropCallback(window->window, drop_callback);
	glfwSetKeyCallback(window->window, key_callback);
	
	image = getImage(imageShader);
	if (image == NULL) return 1;
	
	particleSystem = getParticles(particleShader);
	if (particleSystem == NULL) return 1;
	
	glfwSetMouseButtonCallback(window->window, mouse_button_callback);
	
	return loop(imageShader, particleShader);
}
