#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shaders/shader.h"

#include <string>
#include <vector>

#include <iostream>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

struct Material {
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};
class Mesh {
public:
	// mesh Data
	vector<Vertex>          vertices;
	vector<unsigned int>    indices;
	vector<Texture>			textures;
	unsigned int			VAO;
	Material*				material;
	float 					shininess;
	
	
	
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, aiMaterial* material)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
//		this->material = material;
		
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
//		aiMaterialProperty** properties = material->mProperties;
//		for (int j = 0; j < material->mNumProperties; j++) {
//			cout << properties[j]->mKey.C_Str() << ": " << properties[j]->mData << std::endl;
//		}
//		float shininess;
		//		try{
//		float 					shininess;
//		glm::vec3				diffuse;
//		glm::vec3				specular;

		aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, NULL);
//		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse, NULL);
//		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular, NULL);
//		cout << "shininess: " << shininess << std::endl;
//		this->material = {
//
//		}
	}
	
	// render the mesh
	void Draw(Shader &shader)
	{
		
//		aiColor4D spec_color;
//		if(AI_SUCCESS != aiGetMaterialColor(this->material, AI_MATKEY_COLOR_SPECULAR,
//											&spec_color))
//		{
//			spec_color = aiColor4D(1.f, 1.f, 1.f, 1.f);
//		}
//
		
//		aiUVTransform trafo;
//		unsigned int max = sizeof(aiUVTransform);
//		if (AI_SUCCESS != aiGetMaterialFloatArray(material, AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE,0),
//												  (float*)&trafo, &max) || sizeof(aiUVTransform) != max)
//		{
//			// error handling
//		}
		
//		float shininess;
////		try{
//			aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, NULL);
////			if(AI_SUCCESS != aiGetMaterialFloatArray(this->material, AI_MATKEY_SHININESS, &shininess))
////			{
////				// if unsuccessful set a default
////				shininess = 20.f;
////			}}
////		catch(const std::exception& ex) {cout << "exc" << std::endl;}
		shader.setFloat("material.shininess", shininess);
		
		// bind appropriate textures
		unsigned int diffuseNr  = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr   = 1;
		unsigned int heightNr   = 1;
		for(unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if(name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if(name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to string
			else if(name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to string
			else if(name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to string
			
			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}
	
private:
	// render data
	unsigned int VBO, EBO;
	
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		
		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		
		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
		glBindVertexArray(0);
	}
};
#endif
