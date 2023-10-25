//
//  ParticleSystem.hpp
//  gl from scratch
//
//  Created by Isobel Mahon on 09/05/2023.
//

#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <stdio.h>
#include "Particle.hpp"

//#include "stbInclude.h"
#include <vector>
#include <random>
#include "Image.hpp"


class ParticleSystem {
public:
	ParticleSystem(Shader* s, mat4 projection, const char* brushPath, Image *image);
	std::vector<Particle> particles;
	int numParticles = 1000;
	float size = 15.0f, speed = 5.0f;
	
	void updateParticles();
	void drawParticles();
	
private:
	mat4 projection;
	Shader* shader;
	unsigned int brushTexture;
	Image* image;
	
	void loadBrush(const char* brushPath);
	void initParticles();
	Particle newParticle();
	std::default_random_engine generator;

};

#endif /* ParticleSystem_hpp */
