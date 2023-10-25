//
//  Particle.cpp
//  gl from scratch
//
//  Created by Isobel Mahon on 09/05/2023.
//

#include "Particle.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>

Particle::Particle(float x, float y, vec3 c, vec2 v, Image* i) {
	this->x = x;
	this->y = y;
	velocity.x = v.y;
	velocity.y = v.x;
	colour = c;
	image = i;
	
	unsigned int VBO;
	float vertices[] = {
		// pos      	// tex
		-0.5f, 0.5f, 	0.0f, 1.0f,
		0.5f, -0.5f, 	1.0f, 0.0f,
		-0.5f, -0.5f, 	0.0f, 0.0f,
		
		-0.5f, 0.5f, 	0.0f, 1.0f,
		0.5f, 0.5f, 	1.0f, 1.0f,
		0.5f, -0.5f, 	1.0f, 0.0f
	};
	
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Particle::draw(Shader *shader, float size) {
	shader->use();
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(x, y, 0.0f));
	
	float scaleFactor = size / log(length(velocity) * 1000);
	model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, 1.0f));
	
	shader->setMat4("model", model);
	shader->SetVector4f("spriteColor", vec4(colour, length(velocity) * 5));
	
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

bool Particle::update(vec2 localVelocity, vec3 localColour, float speed) {
	velocity = velocity * (1 - learningRate) + localVelocity * learningRate;
	colour = colour * (1 - learningRate) + localColour * learningRate;
	
	x += velocity.x * speed;
	y += velocity.y * speed;
	
	if (length(velocity) < 0.002)
		return false;
	else return true;
}

