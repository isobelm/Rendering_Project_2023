//
//  Particle.hpp
//  gl from scratch
//
//  Created by Isobel Mahon on 09/05/2023.
//

#ifndef Particle_hpp
#define Particle_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "Image.hpp"

using namespace std;
using namespace glm;

class Particle {
public:
	Particle(float x, float y, vec3 c, vec2 v, Image *image);
	float x, y;
	vec2 velocity;
	vec3 colour;
	float learningRate = 0.05;
	Image *image;
//	mat4 projection;
	
	unsigned int quadVAO;
	
	bool update(vec2 localVelocity, vec3 localColour, float speed);
	
	void draw(Shader *shader, float size);
};

#endif /* Particle_hpp */
