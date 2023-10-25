//
//  Image.cpp
//  Rendering
//
//  Created by Isobel Mahon on 10/04/2023.
//

#include "Image.hpp"

#include "Libraries/stb_image.h"

Image::Image(char const *path, Shader *shader) {
	this->shader = shader;
	int w, h, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	unsigned char *data = stbi_load(path, &w, &h, &nrChannels, 0);
	width = w;
	height = h;
	numberChannels = nrChannels;
	
	unsigned char filtered[w * h * nrChannels];
	
	filterImage(data, filtered);
	
	unsigned char *blurred = blur(filtered);
	
	createTexture(data);
	
	original = data;
	dynamics = blurred;
	
//	stbi_image_free(data);

}

vector<unsigned char> Image::pixelFromAt(unsigned char *image, int x, int y) {
	int yPos = ((height - 1) * width * numberChannels) - y * width * numberChannels;
	int xPos = x * numberChannels;
	
	vector<unsigned char> pixel = {0,0,0};
	pixel.at(0) = image[yPos + xPos];
	pixel.at(1) = image[yPos + xPos + 1];
	pixel.at(2) = image[yPos + xPos + 2];
	
	return pixel;
}

void Image::filterImage(unsigned char* original, unsigned char* output) {
//	for (int i = 0; i < 1; i ++)
	for (int j = 0; j < height; j++) {
		int yPos = j * width * numberChannels;
		for (int i = 0; i < width; i++) {
			int xPos = i * numberChannels;
			
			if (i == 0 or i == width - 1 or j == 0 or j == height - 1) {
				output[yPos + xPos] = 128;
				output[yPos + xPos + 1] = 128;
				output[yPos + xPos + 2] = 50;
			}
			else {
				int north = ((j - 1) * width * numberChannels) + xPos;
				int south = ((j + 1) * width * numberChannels) + xPos;
				int east = yPos + ((i - 1) * numberChannels);
				int west = yPos + ((i + 1) * numberChannels);
				
				int rdx = original[north] - original[south];
				int rdy = original[east] - original[west];
				int gdx = original[north + 1] - original[south + 1];
				int gdy = original[east + 1] - original[west + 1];
				int bdx = original[north + 2] - original[south + 2];
				int bdy = original[east + 2] - original[west + 2];
				
				int dx = (rdx + gdx + bdx) / 3 + 128;
				int dy = (rdy + gdy + bdy) / 3 + 128;
				if (dx > 255) dx = 255;
				if (dy > 255) dy = 255;
				if (dx < 0) dx = 0;
				if (dy < 0) dy = 0;
				output[yPos + xPos] = dx;
				output[yPos + xPos + 1] = dy;
				output[yPos + xPos + 2] = 50;
			}

		}
	}
	
}

void Image::bindBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0); 
}

void Image::createTexture(unsigned char *data) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
}

void Image::draw() {
	shader->use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

vec3 Image::colourAt(int x, int y) {
	vector<unsigned char> rawPixel = pixelFromAt(original, x, y);
	vec3 colour = vec3(0.0f);
	
	colour.x = rawPixel[0] / 255.0f;
	colour.y = rawPixel[1] / 255.0f;
	colour.z = rawPixel[2] / 255.0f;
	
	return colour;
}

vec2 Image::changesAt(int x, int y) {
	vector<unsigned char> rawPixel = pixelFromAt(dynamics, x, y);
	vec2 changes = vec2(0.0f);
	changes.x = ((float)((int)rawPixel[0] - 128 + xBias))/ 255.0f;
	changes.y = ((float)((int)rawPixel[1] - 128 + yBias) )/ 255.0f;
	
	return changes;
}

int Image::coordToInt(int i, int j) {
	int yPos = j * width * numberChannels;
	int xPos = i * numberChannels;
	return yPos + xPos;
}

unsigned char* Image::blur(unsigned char *image) {
	unsigned char* blurred = new unsigned char[width * height * numberChannels];
	
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			for (int k = 0; k < numberChannels; k++) {
				if (i == 0 or i == width or j == 0 or j == height) {
					blurred[coordToInt(i, j) + k] = image[coordToInt(i, j) + k];
				}
				else {
					int avg = image[coordToInt(i - 1, j) + k] + image[coordToInt(i + 1, j) + k] + image[coordToInt(i, j) + k] +
					image[coordToInt(i - 1, j + 1) + k] + image[coordToInt(i + 1, j + 1) + k] + image[coordToInt(i, j + 1) + k] +
					image[coordToInt(i - 1, j - 1) + k] + image[coordToInt(i + 1, j  - 1) + k] + image[coordToInt(i, j -1) + k];
					blurred[coordToInt(i, j) + k] = avg / 9;

				}
			}
		}
	}

	return blurred;
}

void Image::freeData() {
	stbi_image_free(original);
}
