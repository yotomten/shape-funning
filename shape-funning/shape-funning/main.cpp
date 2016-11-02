
#include <iostream>

//GlEW, before GLFW

#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
// Should make it cross platform

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Shaders
const GLchar* vertexShaderSource =
	"#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
	"}\0";

const GLchar* fragmentShaderSource =
	"#version 330 core\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"	color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n";

void	InitShaders()
{

}

int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW and set all its required options
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGl 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't use old functionality
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Necessary for Mac OSX

	// Init GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);

	// Init GLEW to setup function pointers for OpenGl

	glewExperimental = GL_TRUE; // Use core profile modern OpenGL techniques
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init GLEW" << std::endl;
		return -1;
	}

	// Define width and height for OpenGL (viewport dimensions)
	// OpenGL uses this data to map from processed coordinates in range (-1,1) to (0,800) and (0,600)

	int width, height;
	glfwGetFramebufferSize(window, &width, &height); // Framebuffer size in pixels instead of screen coordinates
	glViewport(0, 0, width, height); // (0, 0) = Southwest corner of window

	// Triangle vertices as normalized device coordinates
	GLfloat vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	// Init triangle VBO to store vertices in GPU memory
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies vertex data to GPU
																			   // GL_STATIC_DRAW since the data most likely
																			   // will not change
	// Compile vertex shader
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 1 string, attach source code
	glCompileShader(vertexShader);

	// Check for compile errors
	GLint vertexShaderSuccess;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderSuccess); // Check if shader compilation successful
	if (!vertexShaderSuccess)
	{
		glGetShaderInfoLog(vertexShader,512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Compile fragment shader
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check for compile errors
	GLint fragmentShaderSuccess;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderSuccess); // Check if shader compilation successful
	if (!fragmentShaderSuccess)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // Check if events have been activated

		// Rendering commands
		glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwTerminate(); // Clear allocated resources
	return 0;
}