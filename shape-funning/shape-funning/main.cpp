
#include <iostream>

//GlEW, before GLFW

#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
// Should make it cross platform

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL
#include <soil/SOIL.h>

// GL includes
#include "Classes/Shader.h"
#include "Classes/Model.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int textureWidth, textureHeight;


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void DrawPolygon(std::string type, Shader &shader,
				 const GLuint &VAO, const bool &wireFramed,
				 bool drawWithTexture, GLuint &texture1, GLuint &texture2)
{
	shader.Use();
	if (drawWithTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture2"), 1);
	}

	glBindVertexArray(VAO);

	if (wireFramed)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (type == "triangle")
	{
		glDrawArrays(GL_TRIANGLES, 0, 3); // Arg1 0 start index of vertex array
										  // Arg2 3 vertices are to be drawn
	}
	else if (type == "rectangle")
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Arg2 6 indices
															 // Arg4 0 offset in EBO
	}
	else if (type == "cube")
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else
	{
		std::cout << "Wrong type of polygon in call to DrawPolygon(). " << std::endl;
	}

	glBindVertexArray(0);
}

bool InitGlfwAndGlew(GLFWwindow* &window)
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW and set all its required options
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGl 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't use old functionality
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Necessary for Mac OSX

	// Init GLFWwindow object
	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);

	// Init GLEW to setup function pointers for OpenGl

	glewExperimental = GL_TRUE; // Use core profile modern OpenGL techniques
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init GLEW" << std::endl;
		return false;
	}

	// Define width and height for OpenGL (viewport dimensions)
	// OpenGL uses this data to map from processed coordinates in range (-1,1) to (0,800) and (0,600)
	int width, height;
	glfwGetFramebufferSize(window, &width, &height); // Framebuffer size in pixels instead of screen coordinates
	glViewport(0, 0, width, height); // (0, 0) = Southwest corner of window

	return true;
}

void RotatePolygon(glm::mat4 model, GLuint &modelLoc, Shader &shader, double time)
{
	model = glm::rotate(model, (GLfloat)time * 50.0f, glm::vec3(0.5, 1.0, 0.0));

	shader.Use();
	modelLoc = glGetUniformLocation(shader.Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // GL_FALSE means not transpose

}

void InitGeometryTransformations(glm::mat4 &model, GLuint &modelLoc,
							   Shader &shader, GLfloat scale, GLfloat degrees,
							   glm::mat4 &view, GLuint &viewLoc, glm::mat4 &proj, GLuint &projLoc)
{

	proj = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	model = glm::rotate(model, degrees, glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(scale, scale, scale));

	shader.Use();
	modelLoc = glGetUniformLocation(shader.Program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // GL_FALSE means not transpose

	viewLoc = glGetUniformLocation(shader.Program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projLoc = glGetUniformLocation(shader.Program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
}

void InitTexture(const char* path, GLuint &texture)
{
	unsigned char* image;

	// Load image from file
	image = SOIL_load_image(path, &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	
	if (image == NULL)
	{
		std::cout << "Error: Failed to load image: " << path << std::endl;
	}

	// Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0,
				 GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind
}

void InitHandleVertexInformation(GLuint &VBO,GLuint &VAO,GLuint &EBO)
{

	// Init triangle VBO to store vertices in GPU memory, rectangle EBO to index vertices
	// and VAO to collect all states

	const GLuint* vertices[3]; // Just since model

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies vertex data to GPU
	// GL_STATIC_DRAW since the data most likely
	// will not change
	// The VBO is stored in VAO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Stored in VAO
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Specify how vertex data is to be interpreted...........................................
	// Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0); // Arg1 0 since position is layout 0
																					  // Arg2 3 since position data vec3
																					  // Arg4 false since already normalized values
																					  // Arg5 Space between attribute sets
																					  // Arg6 No data offset in buffer 
	glEnableVertexAttribArray(0); // Vertex attribute location is 0 for position

	// Color attributes
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1); // Vertex attribute location is 1 for color

	// Texture attributes

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind vertex array to not risk misconfiguring later on
}

int main()
{
	GLFWwindow* window;
	bool initGlfwGlewSuccess;
	initGlfwGlewSuccess = InitGlfwAndGlew(window);
	if (!initGlfwGlewSuccess) { return -1; }

	glEnable(GL_DEPTH_TEST);

	Shader modelShader("./Shaders/model_loading.vert", "./Shaders/model_loading.frag");

	Model ourModel("./Models/nanosuit/nanosuit.obj");

	// Init transformations and matrices
	glm::mat4 model, view, proj;
	GLuint modelLoc, viewLoc, projLoc;
	InitGeometryTransformations(model, modelLoc, modelShader, 0.2f, -65.0f,
							   view, viewLoc, proj, projLoc);

	// Ugly deformation
	int firstIteration = 1;
	double oldTime= 0;
	double currentTime;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // Check if events have been activated

		// Rendering commands
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rotate over time
		RotatePolygon(model, modelLoc, modelShader, glfwGetTime());

		/*if (firstIteration == 100)
		{
			ourModel.DeformModel();
		}
		firstIteration++;

		if (firstIteration > 300)
		{
			currentTime = glfwGetTime();
			double timeDiff = currentTime - oldTime;
			ourModel.RestoreDeformedModel(timeDiff);
			oldTime = currentTime;
		}*/
		ourModel.Draw(modelShader);

		glfwSwapBuffers(window);
	}
	glfwTerminate(); // Clear allocated resources
	return 0;
}
