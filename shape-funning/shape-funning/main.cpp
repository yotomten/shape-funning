
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

#include "Shader.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void DrawPolygon(std::string type, Shader shader,
				 const GLuint &VAO, const bool &wireFramed)
{
	shader.Use();
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

void GlmPlay()
{
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 0.0f);
	std::cout << vec.x << vec.y << vec.z << std::endl;
	glm::mat4 trans;
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = trans * vec;
	std::cout << vec.x << vec.y << vec.z << std::endl;
}

int main()
{
	GlmPlay();

	GLFWwindow* window;
	bool initGlfwGlewSuccess;
	initGlfwGlewSuccess = InitGlfwAndGlew(window);
	if (!initGlfwGlewSuccess) { return -1; }

	// Triangle vertices as normalized device coordinates

	GLfloat vertices[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // Bottom Left
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // Top 
	};

	GLfloat texCoords[] = {
		0.0f, 0.0f,  // Lower-left corner  
		1.0f, 0.0f,  // Lower-right corner
		0.5f, 1.0f   // Top-center corner
	};

	GLuint indices[6] =
	{
		0, 1, 3,	// First triangle
		1, 2, 3		// Second triangle
	};

	// Init triangle VBO to store vertices in GPU memory, rectangle EBO to index vertices
	// and VAO to collect all states
	GLuint VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies vertex data to GPU
																			   // GL_STATIC_DRAW since the data most likely
																			   // will not change
																			   // The VBO is stored in VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Stored in VAO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Specify how vertex data is to be interpreted...........................................
	// Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Arg1 0 since position is layout 0
																					  // Arg2 3 since position data vec3
																					  // Arg4 false since already normalized values
																					  // Arg5 Space between attribute sets
																					  // Arg6 No data offset in buffer 
	glEnableVertexAttribArray(0); // Vertex attribute location is 0 for position

	// Color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // Vertex attribute location is 1 for color
	glBindVertexArray(0); // Unbind vertex array to not risk misconfiguring later on

	Shader simpleShader("./shader.vert",
		"./shader.frag");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // Check if events have been activated

		// Rendering commands
		glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bool wireFramed = false;
		DrawPolygon("triangle", simpleShader, VAO, wireFramed);

		glfwSwapBuffers(window);
	}

	// Deallocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate(); // Clear allocated resources
	return 0;
}