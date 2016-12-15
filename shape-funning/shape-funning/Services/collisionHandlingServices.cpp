
#include "../Classes/Model.h"
#include <vector>

namespace collisionHandlingServices
{
	bool ModelsColliding(Model modelA, glm::mat4 modelMatrixA,
		Model modelB, glm::mat4 modelMatrixB)
	{
		glm::vec3 vertexA = glm::vec3(modelMatrixA * glm::vec4(modelA.meshes[0].vertices[0].Position, 1.0f));
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
						vertexB = glm::vec3(modelMatrixB * glm::vec4(modelB.meshes[k].vertices[l].Position, 1.0f));
						diff = glm::dot(face, vertexA) - glm::dot(face, vertexB);
						//std::cout << "diff A -> B: " << diff << endl;

						if (diff > 0)
						{
							hit = true;
							//std::cout << "hit: " << hit << endl;
						}
					}
				}
			}
		}

		if (!hit)
		{
			return false;
		}

		vertexB = glm::vec3(modelMatrixB * glm::vec4(modelB.meshes[0].vertices[0].Position, 1.0f));

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
						vertexA = glm::vec3(modelMatrixA * glm::vec4(modelA.meshes[k].vertices[l].Position, 1.0f));
						diff = glm::dot(face, vertexB) - glm::dot(face, vertexA);
						//std::cout << "diff B -> A: " << diff << endl;

						if (diff > 0)
						{
							hit = true;
							//std::cout << "hit: " << hit << endl;
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

	//bool CollidingWithFloor(Model modelA, glm::mat4 modelMatrixA)
	//{
	//	glm::vec3 vertexA = glm::vec3(modelMatrixA * glm::vec4(modelA.meshes[0].vertices[0].Position, 1.0f));

	//	vector<glm::vec3> floorVertices;
	//	glm::vec3 vertex = glm::vec3(10.0f, -10.0f, 10.0f);
	//	floorVertices.push_back(vertex);
	//	vertex = glm::vec3(10.0f, -10.0f, -10.0f);
	//	floorVertices.push_back(vertex);
	//	vertex = glm::vec3(-10.0f, -10.0f, -10.0f);
	//	floorVertices.push_back(vertex);
	//	vertex = glm::vec3(-10.0f, -10.0f, 10.0f);
	//	floorVertices.push_back(vertex);

	//	glm::vec3 floorNormal = glm::vec3(0.0f, 1.0f, 0.0f);

	//	glm::vec3 vertexB;
	//	GLfloat diff;
	//	glm::vec3 face;
	//	bool hit = false;

	//	for (GLuint i = 0; i < modelA.meshes.size(); i++)
	//	{
	//		for (GLuint j = 0; j < modelA.meshes[i].vertices.size(); j += 4)
	//		{
	//			// For all faces
	//			face = modelA.meshes[i].vertices[j].Normal;
	//			hit = false;

	//			for (GLuint k = 0; k < modelB.meshes.size(); k++)
	//			{

	//				for (GLuint l = 0; l < modelB.meshes[k].vertices.size(); l++)
	//				{
	//					// For all vertices in B
	//					vertexB = glm::vec3(modelMatrixB * glm::vec4(modelB.meshes[k].vertices[l].Position, 1.0f));
	//					diff = glm::dot(face, vertexA) - glm::dot(face, vertexB);
	//					//std::cout << "diff A -> B: " << diff << endl;

	//					if (diff > 0)
	//					{
	//						hit = true;
	//						//std::cout << "hit: " << hit << endl;
	//					}
	//				}
	//			}
	//		}
	//	}

	//	if (!hit)
	//	{
	//		return false;
	//	}

	//	vertexB = glm::vec3(modelMatrixB * glm::vec4(modelB.meshes[0].vertices[0].Position, 1.0f));

	//	for (GLuint i = 0; i < modelB.meshes.size(); i++)
	//	{
	//		for (GLuint j = 0; j < modelB.meshes[i].vertices.size(); j += 4)
	//		{
	//			// For all faces
	//			face = modelB.meshes[i].vertices[j].Normal;
	//			hit = false;

	//			for (GLuint k = 0; k < modelA.meshes.size(); k++)
	//			{

	//				for (GLuint l = 0; l < modelA.meshes[k].vertices.size(); l++)
	//				{
	//					// For all vertices in A
	//					vertexA = glm::vec3(modelMatrixA * glm::vec4(modelA.meshes[k].vertices[l].Position, 1.0f));
	//					diff = glm::dot(face, vertexB) - glm::dot(face, vertexA);
	//					//std::cout << "diff B -> A: " << diff << endl;

	//					if (diff > 0)
	//					{
	//						hit = true;
	//						//std::cout << "hit: " << hit << endl;
	//					}
	//				}
	//			}

	//		}
	//	}


	//	if (!hit)
	//	{
	//		return false;
	//	}

	//	return true;

	//}

	bool CollideWithFloor(Model &model, glm::mat4 modelMatrix, glm::vec3 &speed, glm::vec3 &acceleration)
	{
		glm::vec3 vertex;
		bool hit = false;

		for (GLuint i = 0; i < model.meshes.size(); i++)
		{
			for (GLuint j = 0; j < model.meshes[i].vertices.size(); j ++)
			{
				vertex = glm::vec3(modelMatrix * glm::vec4(model.meshes[i].vertices[j].Position, 1.0f));

				//std::cout << vertex.y << endl;

				if (vertex.y <= -10.0f)
				{
					model.meshes[i].vertices[j].Velocity.y += 2.0f * glm::abs(glm::vec3(modelMatrix * glm::vec4(model.meshes[i].vertices[j].Position, 1.0f)).y - 10.0f);
					
					std::cout << "collision" << endl;
					speed = glm::vec3(0.0f);
					acceleration = glm::vec3(0.0f);

					hit = true;
				}
			}
			model.meshes[i].setupMesh();
		}

		return hit;
	}
}