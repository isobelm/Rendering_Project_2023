//
//  PointLight.cpp
//  gl from scratch
//
//  Created by Isobel Mahon on 30/01/2023.
//

#include "PointLight.hpp"
#include <glm/glm.hpp>


class PointLight {
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    PointLight() {
        this->position = glm::vec3(0,0,0);
        this->ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        this->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        this->specular = glm::vec3(1.0f, 1.0f, 1.0f);
        this->constant = 1.0f;
        this->linear = 0.09f;
        this->quadratic = 0.032f;
    }
};
