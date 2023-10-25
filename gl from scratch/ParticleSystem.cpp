//
//  ParticleSystem.cpp
//  gl from scratch
//
//  Created by Isobel Mahon on 09/05/2023.
//

#include "ParticleSystem.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include "Libraries/stb_image.h"

#include <random>
//#include "stbInclude.h"

ParticleSystem::ParticleSystem(Shader* s, mat4 projection, const char* brushPath, Image *i) {
	shader = s;
	image = i;
	this->projection = projection;
	
//	particles = new Particle();
//	particles =
	
	initParticles();
	loadBrush(brushPath);
}

Particle ParticleSystem::newParticle() {
	uniform_real_distribution<float> randomX(1.0f, (float)image->width - 1.0f);
	uniform_real_distribution<float> randomY(1.0f, (float)image->height - 1.0f);
//	uniform_real_distribution<float> randomY(300.0f, (float)image->height - 1.0f);
	
	float x = randomX(generator);
	float y = randomY(generator);
	return Particle(x,y, image->colourAt(x, y), image->changesAt(x, y), image);
}

void ParticleSystem::initParticles() {
	for (int i = 0; i < numParticles; i++) {
		particles.push_back(newParticle());
	}
}

void ParticleSystem::loadBrush(const char* brushPath) {
	int w, h, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char *data = stbi_load(brushPath, &w, &h, &nrChannels, 0);
	
	glGenTextures(1, &brushTexture);
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(data);
}

void ParticleSystem::drawParticles() {
	shader->use();
	shader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	
	for (int i = 0; i < particles.size(); i++) {
		particles.at(i).draw(shader, size);
	}
	
}

void ParticleSystem::updateParticles() {
	for (int i = 0; i < particles.size(); i++) {
		Particle *p = &particles.at(i);
		if ((int) p->x < 1 or (int) p->x > image->width - 2 or
			(int) p->y < 1 or (int) p->y > image->height - 2){
			particles.at(i) = newParticle();
		}
		else {
			if (!p->update(image->changesAt((int)p->x, (int)p->y), image->colourAt((int)p->x, (int)p->y), speed)) {
				particles.at(i) = newParticle();
			}
		}
	}
}
