
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
"layout (location = 1) in vec3 color;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0);\n"
"ourColor = color;\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(ourColor, 1.0f);\n"
"}\0";

void	InitShaders(GLuint &vertexShader, GLuint &fragmentShader)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 1 string, attach source code
	glCompileShader(vertexShader);

	// Check for compile errors
	GLint vertexShaderSuccess;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderSuccess); // Check if shader compilation successful
	if (!vertexShaderSuccess)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Compile fragment shader
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
}

void InitShaderProgram(GLuint &shaderProgram, GLuint vertexShader, GLuint fragmentShader)
{
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for link errors
	GLint shaderProgramSuccess;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderProgramSuccess);
	if (!shaderProgramSuccess)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADEPROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}

void DrawPolygon(std::string type, const GLuint &shaderProgram, const GLuint &VAO, const bool &wireFramed)
{
	glUseProgram(shaderProgram);
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

int main()
{
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
	glEnableVertexAttribArray(0); // Vertex attribute location is 0

	// Color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0); // Unbind vertex array to not risk misconfiguring later on

	// Compile shaders
	GLuint vertexShader, fragmentShader;
	InitShaders(vertexShader, fragmentShader);

	// Attach shaders and link shader program
	GLuint shaderProgram;
	InitShaderProgram(shaderProgram, vertexShader, fragmentShader);
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // Check if events have been activated

		// Color animation
		/*GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

		// Rendering commands
		glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bool wireFramed = false;
		DrawPolygon("triangle", shaderProgram, VAO, wireFramed);

		glfwSwapBuffers(window);
	}

	// Deallocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate(); // Clear allocated resources
	return 0;
}