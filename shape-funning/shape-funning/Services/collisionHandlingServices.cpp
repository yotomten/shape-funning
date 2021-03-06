
#include "../Classes/Model.h"
#include <vector>

namespace collisionHandlingServices
{
	// Separating Axis theorem. Works for now only on specific cubes, since faces is fetched in a certain way
	bool ModelsColliding(Model modelA, Model modelB)
	{
		glm::vec3 vertexA = glm::vec3(modelA.modelMatrix * glm::vec4(modelA.meshes[0].vertices[0].Position, 1.0f));
		glm::vec3 vertexB;
		GLfloat diff;
		glm::vec3 face;
		bool hit = false;

		for (GLuint i = 0; i < modelA.meshes.size(); i++)
		{
			for (GLuint j = 0; j < modelA.meshes[i].vertices.size(); j+=4)
			{
				// For all faces
				face = modelA.meshes[i].vertices[j].Normal;
				hit = false;

				for (GLuint k = 0; k < modelB.meshes.size(); k ++)
				{

					for (GLuint l = 0; l < modelB.meshes[k].vertices.size(); l++)
					{
						// For all vertices in B
						vertexB = glm::vec3(modelB.modelMatrix * glm::vec4(modelB.meshes[k].vertices[l].Position, 1.0f));
						diff = glm::dot(face, vertexA) - glm::dot(face, vertexB);

						if (diff > 0)
						{
							hit = true;
						}
					}
				}
			}
		}

		if (!hit)
		{
			return false;
		}

		vertexB = glm::vec3(modelB.modelMatrix * glm::vec4(modelB.meshes[0].vertices[0].Position, 1.0f));

		for (GLuint i = 0; i < modelB.meshes.size(); i++)
		{
			for (GLuint j = 0; j < modelB.meshes[i].vertices.size(); j += 4)
			{
				// For all faces
				face = modelB.meshes[i].vertices[j].Normal;
				hit = false;

				for (GLuint k = 0; k < modelA.meshes.size(); k++)
				{

					for (GLuint l = 0; l < modelA.meshes[k].vertices.size(); l++)
					{
						// For all vertices in A
						vertexA = glm::vec3(modelA.modelMatrix * glm::vec4(modelA.meshes[k].vertices[l].Position, 1.0f));
						diff = glm::dot(face, vertexB) - glm::dot(face, vertexA);

						if (diff > 0)
						{
							hit = true;
						}
					}
				}

			}
		}


		if (!hit)
		{
			return false;
		}

		return true;

	}

	// Simple collision detection and response, hardcoded floor
	void CollideWithFloor(Model &model, glm::mat4 modelMatrix, glm::vec3 &speed, glm::vec3 &acceleration)
	{
		glm::vec3 vertex;

		for (GLuint i = 0; i < model.meshes.size(); i++)
		{
			for (GLuint j = 0; j < model.meshes[i].vertices.size(); j ++)
			{
				vertex = glm::vec3(modelMatrix * glm::vec4(model.meshes[i].vertices[j].Position, 1.0f));

				if (vertex.y <= -10.0f)
				{
					// Inner velocity will cause deformation
					model.meshes[i].vertices[j].Velocity.y += 4.0f * glm::abs(glm::vec3(modelMatrix * glm::vec4(model.meshes[i].vertices[j].Position, 1.0f)).y - 10.0f);
					speed = glm::vec3(0.0f);
					acceleration = glm::vec3(0.0f);
				}
			}
			model.meshes[i].setupMesh();
		}
	}
}