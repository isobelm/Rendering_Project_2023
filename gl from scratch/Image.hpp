//
//  Image.hpp
//  Rendering
//
//  Created by Isobel Mahon on 10/04/2023.
//

#ifndef Image_hpp
#define Image_hpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <vector>
#include "Shader.hpp"


using namespace std;
using namespace glm;

class Image {
public:
	Image(char const *path, Shader *shader);
	
	int width, height, numberChannels;
	int xBias = 0, yBias = 0;
	unsigned int texture;

	void bindBuffers();
	void createTexture(unsigned char *data);
	void draw();
	vec3 colourAt(int x, int y);
	vec2 changesAt(int x, int y);
	vector<unsigned char> pixelFromAt(unsigned char *image, int x, int y);
	
	void freeData();
	
private:
	float vertices[32] = {
		// positions           	// texture coords
		1.0f,  1.0f, 0.0f,      1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,		0.0f, 1.0f  // top left
	};
	unsigned int indices[6] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	void filterImage(unsigned char* original, unsigned char* output);
	unsigned char *blur(unsigned char* image);
	int coordToInt(int x, int y);

	unsigned int VBO, VAO, EBO;
	Shader *shader;
	unsigned char *original;
	unsigned char *dynamics;

};

#endif /* Image_hpp */
