#pragma once

#include "Model.h"

class CollisionHandler
{
public:

	CollisionHandler();
	~CollisionHandler();

	bool ModelsColliding(Model modelA, Model modelB)
	{
		glm::vec3 vertexA = modelA.meshes[0].vertices[0].Position;
		glm::vec3 vertexB;
		GLfloat diff;
		glm::vec3 face;
		bool hit;

		for (GLuint i = 0; i < modelA.meshes.size(); i++)
		{
			for (GLuint j = 0; j < modelA.meshes[i].vertices.size(); j+=4)
			{
				face = modelA.meshes[i].vertices[j].Normal;

				for (GLuint k = 0; k < modelB.meshes.size(); k ++)
				{

					for (GLuint l = 0; l < modelB.meshes[k].vertices.size(); l++)
					{
						vertexB = modelB.meshes[k].vertices[l].Position;
						diff = glm::dot(face, vertexA) - glm::dot(face, vertexB);

						if (diff > 0)
						{
							std::cout << "hit: " << hit <<  endl;
							hit = true;
						}
						else
						{
							return false; // Found separating plane
						}
					}

				}

			}
		}

		vertexB = modelA.meshes[0].vertices[0].Position;

		for (GLuint i = 0; i < modelB.meshes.size(); i++)
		{
			for (GLuint j = 0; j < modelB.meshes[i].vertices.size(); j += 4)
			{
				face = modelB.meshes[i].vertices[j].Normal;

				for (GLuint k = 0; k < modelA.meshes.size(); k++)
				{

					for (GLuint l = 0; l < modelA.meshes[k].vertices.size(); l++)
					{
						vertexA = modelA.meshes[k].vertices[l].Position;
						diff = glm::dot(face, vertexB) - glm::dot(face, vertexA);

						if (diff > 0)
						{
							std::cout << "hit: " << hit << endl;
							hit = true;
						}
						else
						{
							return false; // Found separating plane
						}
					}

				}

			}
		}

		return true;

	}


};