//
//  Window.cpp
//  Rendering
//
//  Created by Isobel Mahon on 10/04/2023.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.hpp"

#include <iostream>

using namespace std;

void Window::createWindow(int width, int height) {
	window = glfwCreateWindow(width, height, "Graphics", NULL, NULL);
//	filePath = NULL;
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
	}
	else {
		glfwMakeContextCurrent(window);
	}
}

bool Window::initGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	else return true;
}

Window::Window(int width, int height) {
	
	createWindow(width, height);
	if (window != NULL) {
		
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
		
		if (!initGlad()) window = NULL;
		
		//	stbi_set_flip_vertically_on_load(false);
		
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, width, height);
	}
}

void Window::setSize(int width, int height) {
	glfwSetWindowSize(window, width, height);
}


void Window::processInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
