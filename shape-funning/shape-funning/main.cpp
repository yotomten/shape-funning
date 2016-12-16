
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

// Other includes
#include "Classes/Shader.h"
#include "Classes/Camera.h"
#include "Classes/Model.h"
#include "Services/CollisionhandlingServices.cpp"

// GLOBALS..............................................................

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// For SOIL
int textureWidth, textureHeight;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void DoMovement();

// Camera
Camera camera(glm::vec3(0.0f, 10.0f, 10.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Deformation
GLfloat dampingConstant = 0.15f;
GLfloat alpha = 0.15f;
bool restore = true;

// Integration

GLfloat g = 9.82f;

bool InitGlfwAndGlew(GLFWwindow* &window)
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW and set all its required options
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGl 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't use old functionality
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Necessary for Mac OSX
	
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Init GLFWwindow object
	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);


	// Set callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide and capture cursor

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

void RotateModel(glm::mat4 model, GLuint &modelLoc, Shader &shader, double time)
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

void UpdateUniforms(glm::mat4 &view, GLuint &viewLoc,
					glm::mat4 &proj, GLuint &projLoc,
					Shader &shader, GLfloat deltaTime, GLuint &timeLoc)
{
	// Create camera transformation
	view = camera.GetViewMatrix();
	proj = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);

	shader.Use();
	// Get the uniform locations and pass the matrices to the shader

	viewLoc = glGetUniformLocation(shader.Program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projLoc = glGetUniformLocation(shader.Program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	timeLoc = glGetUniformLocation(shader.Program, "deltaTime");
	glUniform1f(timeLoc, deltaTime);

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

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void MouseCallback(GLFWwindow* window, double xPosition, double yPosition)
{
	GLfloat xPos = (GLfloat)xPosition;
	GLfloat yPos = (GLfloat)yPosition;

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xoffset = xPos - lastX;
	GLfloat yoffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll((GLfloat)yOffset);
}

void HandleDeformation(Model &ourModel, GLfloat time, glm::mat4 &model)
{
	if (keys[GLFW_KEY_R])
	{
		ourModel.DeformModel();
		restore = false;
	}
	if (keys[GLFW_KEY_T])
		restore = true;
	if (keys[GLFW_KEY_Y])
	{
		dampingConstant += 0.001f;
		cout << "dampingConstant: " << dampingConstant << endl;
	}
	if (keys[GLFW_KEY_H])
	{
		if (dampingConstant > 0.001f)
			dampingConstant -= 0.001f;

		cout << "dampingConstant: " << dampingConstant << endl;
	}
	if (keys[GLFW_KEY_U])
	{
		if (alpha < 1.0f)
			alpha += 0.01f;

		if (alpha > 1.0f)
		{
			alpha = 1.0f;
		}

		cout << "alpha: " << alpha << endl;
	}
	if (keys[GLFW_KEY_J])
	{
		if (alpha > 0.02f)
			alpha -= 0.01f;
		cout << "alpha: " << alpha << endl;
	}
	if (keys[GLFW_KEY_C])
	{
		ourModel.GetCentroid();
	}
	if (keys[GLFW_KEY_Q])
	{
		model = glm::rotate(model, 5.0f, glm::vec3(0.0, 0.5, 1.0));
		ourModel.modelMatrix = model;
	}
}

int main()
{
	GLFWwindow* window;
	bool initGlfwGlewSuccess;
	initGlfwGlewSuccess = InitGlfwAndGlew(window);
	if (!initGlfwGlewSuccess) { return -1; }

	glEnable(GL_DEPTH_TEST);

	Shader modelShader("./Shaders/model_loading.vert", "./Shaders/model_loading.frag");

	cout << "Loading model..." << endl;
	Model ourModel("./Models/Crate/crate1.obj");
	cout << "Loaded model. " << endl;
	cout << "Loading model..." << endl;
	Model ourModel2("./Models/Crate/crate1.obj");
	cout << "Loaded model. " << endl;
	cout << "Loading model..." << endl;
	Model ourModel3("./Models/Crate/crate1.obj");
	cout << "Loaded model. " << endl;
	Model referenceModel = ourModel;
	Model containingModel("./Models/Crate2/Crate2.obj");

	// Init transformations
	glm::mat4 model, model2, model3, containingModelMatrix, view, proj;
	GLuint modelLoc, viewLoc, projLoc, timeLoc;
	GLfloat delta = 0;
	model2 = glm::translate(model2, glm::vec3(5.0f, 0.0f, -5.0f));
	model3 = glm::translate(model3, glm::vec3(-5.0f, 0.0f, 5.0f));

	ourModel.modelMatrix = model;
	ourModel2.modelMatrix = model2;
	ourModel3.modelMatrix = model3;
	containingModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	containingModel.modelMatrix = containingModelMatrix;

	// Integration parameters
	glm::vec3 acceleration, speed, positionChange, acceleration2, speed2, positionChange2, acceleration3, speed3, positionChange3;


	// Pre-calculated deformation parameters
	vector<glm::vec3> q;
	q = referenceModel.Findq(&referenceModel);
	glm::mat3 Aqq = referenceModel.FindAqq(q);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Integration

		acceleration += glm::vec3(0.0f, -g, 0.0f);
		speed += acceleration * deltaTime * 0.1f;
		positionChange = speed * deltaTime * 0.1f;
		acceleration2 += glm::vec3(0.0f, -g, 0.0f);
		speed2 += acceleration2 * deltaTime * 0.1f;
		positionChange2 = speed2 * deltaTime * 0.1f;
		acceleration3 += glm::vec3(0.0f, -g, 0.0f);
		speed3 += acceleration3 * deltaTime * 0.1f;
		positionChange3 = speed3 * deltaTime * 0.1f;

		model = glm::translate(model, positionChange);
		model2 = glm::translate(model2, positionChange2);
		model3 = glm::translate(model3, positionChange3);
		ourModel.modelMatrix = model;
		ourModel2.modelMatrix = model2;
		ourModel3.modelMatrix = model3;

		//bool collision = collisionHandlingServices::ModelsColliding(ourModel,
			//model, containingModel, containingModelMatrix);

		collisionHandlingServices::CollideWithFloor(ourModel, ourModel.modelMatrix, speed, acceleration);
		collisionHandlingServices::CollideWithFloor(ourModel2, ourModel2.modelMatrix, speed2, acceleration2);
		collisionHandlingServices::CollideWithFloor(ourModel3, ourModel3.modelMatrix, speed3, acceleration3);

		glfwPollEvents(); // Check if events have been activated
		DoMovement();
		delta += 0.01f;
		// Rendering commands
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		HandleDeformation(ourModel, deltaTime, model);

		UpdateUniforms(view, viewLoc, proj, projLoc, modelShader, delta, timeLoc);

		if (restore)
		{
			ourModel.RestoreDeformedModel(referenceModel, deltaTime, dampingConstant, alpha, Aqq, q, 0.8f);
			ourModel2.RestoreDeformedModel(referenceModel, deltaTime, dampingConstant, 0.7f, Aqq, q, 0.8f);
			ourModel3.RestoreDeformedModel(referenceModel, deltaTime, dampingConstant, 0.05f, Aqq, q, 0.8f);
		}

		ourModel.Draw(modelShader,ourModel.modelMatrix, modelLoc);
		containingModel.Draw(modelShader, containingModel.modelMatrix, modelLoc);
		ourModel2.Draw(modelShader, ourModel2.modelMatrix, modelLoc);
		ourModel3.Draw(modelShader, ourModel3.modelMatrix, modelLoc);



		glfwSwapBuffers(window);
	}
	glfwTerminate(); // Clear allocated resources
	return 0;
}

