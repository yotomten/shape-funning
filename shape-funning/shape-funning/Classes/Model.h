#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessary OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <soil/SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include <vector>

GLint TextureFromFile(const char* path, string directory);

class Model
{
public:
	/*  Model Data  */
	glm::mat4 modelMatrix;
	vector<Mesh> meshes;

	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(GLchar* path)
	{
		this->loadModel(path);
	}

	// Draws the model, and thus all its meshes
	void Draw(Shader shader, glm::mat4 &model, GLuint &modelLoc)
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(shader, model, modelLoc);
	}

	void DeformModel()
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			for (GLuint j = 0; j < meshes[i].vertices.size(); j++)
			{
				if (this->meshes[i].vertices[j].Position.y < 0)
					this->meshes[i].vertices[j].Position.y += 0.02f;
				else
					this->meshes[i].vertices[j].Position.y -= 0.02f;
				if (this->meshes[i].vertices[j].Position.x < 0)
					this->meshes[i].vertices[j].Position.x += 0.02f;
				else
					this->meshes[i].vertices[j].Position.x -= 0.02f;
				if (this->meshes[i].vertices[j].Position.z < 0)
					this->meshes[i].vertices[j].Position.z += 0.02f;
				else
					this->meshes[i].vertices[j].Position.z -= 0.02f;
			}
			this->meshes[i].setupMesh();
		}
	}

	void RestoreDeformedModel(Model &referenceModel, double time, GLfloat k,
		GLfloat alpha, glm::mat3 Aqq, vector<glm::vec3> q)
	{
		glm::vec3 goalPos;
		GLfloat timeStep = (GLfloat)time;
		GLfloat constrainingForce;
		GLfloat displacement;
		glm::vec3 centroid = CalculateCentroid();
		GLfloat beta = 0.8f;

		glm::mat3 Apq = FindApq(q);
		glm::mat3 R = FindR(Apq);
		glm::mat3 A = FindA(Apq, Aqq);



		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			for (GLuint j = 0; j < this->meshes[i].vertices.size(); j++)
			{
				float hej = glm::determinant(R);
				//goalPos = referenceModel.meshes[i].vertices[j].Position;
				goalPos = (beta * A + (1 - beta) * R) * q[j] + centroid;
				displacement = goalPos.y - this->meshes[i].vertices[j].Position.y;
				constrainingForce = -k*this->meshes[i].vertices[j].Velocity.y;

				// Update velocity y
				this->meshes[i].vertices[j].Velocity.y += ((alpha / timeStep) * displacement) + constrainingForce;
				//Update position y
				this->meshes[i].vertices[j].Position.y += timeStep * this->meshes[i].vertices[j].Velocity.y;

				displacement = goalPos.x - this->meshes[i].vertices[j].Position.x;
				constrainingForce = -k*this->meshes[i].vertices[j].Velocity.x;
				// Update velocity x
				this->meshes[i].vertices[j].Velocity.x += ((alpha / timeStep) * displacement) + constrainingForce;
				//Update position x
				this->meshes[i].vertices[j].Position.x += timeStep * this->meshes[i].vertices[j].Velocity.x;

				displacement = goalPos.z - this->meshes[i].vertices[j].Position.z;
				constrainingForce = -k*this->meshes[i].vertices[j].Velocity.z;
				// Update velocity z
				this->meshes[i].vertices[j].Velocity.z += ((alpha / timeStep) * displacement) + constrainingForce;
				//Update position z
				this->meshes[i].vertices[j].Position.z += timeStep * this->meshes[i].vertices[j].Velocity.z;
			}
			this->meshes[i].setupMesh();

		}
	}

	glm::vec3 CalculateCentroid()
	{
		glm::vec3 centroid;
		GLfloat nrOfVertices = 0.0f;

		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			for (GLuint j = 0; j < this->meshes[i].vertices.size(); j++)
			{
				centroid.x += this->meshes[i].vertices[j].Position.x;
				centroid.y += this->meshes[i].vertices[j].Position.y;
				centroid.z += this->meshes[i].vertices[j].Position.z;
			}
			nrOfVertices += this->meshes[i].vertices.size();
		}

		centroid.x /= nrOfVertices;
		centroid.y /= nrOfVertices;
		centroid.z /= nrOfVertices;

		return centroid;
	}

	glm::vec3 CalculateRefCentroid(Model *referenceModel)
	{
		glm::vec3 centroid;
		GLfloat nrOfVertices = 0.0f;

		for (GLuint i = 0; i < referenceModel->meshes.size(); i++)
		{
			for (GLuint j = 0; j < referenceModel->meshes[i].vertices.size(); j++)
			{
				centroid.x += referenceModel->meshes[i].vertices[j].Position.x;
				centroid.y += referenceModel->meshes[i].vertices[j].Position.y;
				centroid.z += referenceModel->meshes[i].vertices[j].Position.z;
			}
			nrOfVertices += referenceModel->meshes[i].vertices.size();
		}

		centroid.x /= nrOfVertices;
		centroid.y /= nrOfVertices;
		centroid.z /= nrOfVertices;

		cout << "Centroid.x : " << centroid.x << endl;
		cout << "Centroid.y : " << centroid.y << endl;
		cout << "Centroid.z : " << centroid.z << endl;

		return centroid;
	}

	glm::vec3 GetCentroid()
	{
		return this->centroid;
	}

	void SetCentroid(glm::vec3 centroid)
	{
		this->centroid = centroid;
	}

	int GetNrOfRefVertices(Model *referenceModel)
	{
		int nrOfVertices;

		for (GLuint i = 0; i < referenceModel->meshes.size(); i++)
		{
			nrOfVertices += referenceModel->meshes[i].vertices.size();
		}
		return nrOfVertices;
	}

	std::vector<glm::vec3> Findq(Model *referenceModel)
	{
		std::vector<glm::vec3> q;
		glm::vec3 centroid = CalculateCentroid();

		for (GLuint i = 0; i < referenceModel->meshes.size(); i++)
		{
			for (GLuint j = 0; j < referenceModel->meshes[i].vertices.size(); j++)
			{
				q.push_back(referenceModel->meshes[i].vertices[j].Position - centroid); // q transpose implied in second case
			}
		}

		return q;
	}

	glm::mat3 FindAqq(vector<glm::vec3> q)
	{
		glm::mat3 Aqq;

		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			for (GLuint j = 0; j < this->meshes[i].vertices.size(); j++)
			{
				Aqq += q[j] * q[j]; // q transpose implied in second case
			}
		}
		Aqq = glm::inverse(Aqq);
		return Aqq;
	}

	glm::mat3 FindApq(vector<glm::vec3> q)
	{
		glm::mat3 Apq;
		glm::vec3 p;
		glm::vec3 centroid = CalculateCentroid();
		glm::vec3 vertex;

		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			for (GLuint j = 0; j < this->meshes[i].vertices.size(); j++)
			{
				vertex = this->meshes[i].vertices[j].Position;
				p = vertex - centroid;
				Apq += p * q[j]; // q transpose implied
			}
		}
		return Apq;
	}


	// Perform Denman–Beavers iteration to find the square root.
	glm::mat3 sqrtMat(glm::mat3 m)
	{
		glm::mat3 m1 = m;
		glm::mat3 m2 = glm::mat3();
		glm::mat3 half_mat;
		half_mat = 0.5f * glm::mat3();

		for (int i = 0; i < 100; ++i)
		{
			glm::mat3 inv1 = glm::inverse(m1);
			glm::mat3 inv2 = glm::inverse(m2);
			m1 = (m1 + inv2) * half_mat;
			m2 = (m2 + inv1) * half_mat;
		}

		return m1;
	}

	glm::mat3 FindR(glm::mat3 Apq)
	{
		glm::mat3 S;
		glm::mat3 R;
		glm::mat3 SInverse;
		glm::mat3 ApqTApq = glm::transpose(Apq) * Apq;

		S = sqrtMat(ApqTApq);

		R = Apq * glm::inverse(S);

		return R;
	}

	glm::mat3 FindA(glm::mat3 Apq, glm::mat3 Aqq)
	{
		glm::mat3 A;
		A = Apq * Aqq;
		A /= glm::pow(glm::determinant(A), 1.0f / 3.0f); // Preserve volume
		return A;
	}

private:
	/*  Model Data  */
	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	glm::vec3 centroid;

	/*  Functions   */
	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// Retrieve the directory path of the filepath
		this->directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		this->processNode(scene->mRootNode, scene);
	}

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene. 
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		// Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Normals
			
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			
			// Texture Coordinates
			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);


			// Vertex velocity
			glm::vec3 velVector; // Zero vector
			vertex.Velocity = velVector;

		}
		// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. Specular maps
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		// Return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			GLboolean skip = false;
			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // If texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};




GLint TextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}