#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


#include <GL/glew.h>

class Shader
{
public:
	// Program ID
	GLuint Program;

	// Constructor reads and build the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		const GLchar* vertexCode = ShaderCodeFromFile(vertexPath, "vertex");
		const GLchar* fragmentCode = ShaderCodeFromFile(fragmentPath, "fragment");

		GLuint vertexShader = CompileShader(vertexCode, "vertex");
		GLuint fragmentShader = CompileShader(fragmentCode, "fragment");

		LinkShaderProgram(vertexShader, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

	}

	// Use the program
	void Use()
	{
		glUseProgram(this->Program);
	}

private:
	const GLchar* ShaderCodeFromFile(const GLchar* shaderPath, std::string shaderType)
	{
		std::string shaderCode;
		std::ifstream shaderFile;

		// Ensures ifstream objects can throw exceptions
		shaderFile.exceptions(std::ifstream::badbit);
		try
		{
			shaderFile.open(shaderPath);
			std::stringstream shaderStream;
			// Read file's buffer contents into streams
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();
			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "Error: Shader of type " + shaderType + " not successfully read. \n" << std::endl;
		}

		// Convert strings into GLchar array
		const GLchar* convertedShaderCode = shaderCode.c_str();
		return convertedShaderCode;
	}

	GLuint CompileShader(const GLchar* shaderCode, std::string shaderType)
	{
		GLuint shader;

		if (shaderType == "vertex")
		{
			shader = glCreateShader(GL_VERTEX_SHADER);
		}
		else if (shaderType == "fragment")
		{
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		}
		else
		{
			std::cout << "Wrong type of shader in call to CompileShader(). " << std::endl;
		}
		
		glShaderSource(shader, 1, &shaderCode, NULL); // 1 string, attach source code
		glCompileShader(shader);

		// Check for compile errors
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Check if shader compilation successful
		if (!success)
		{

			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "Error: Compilation of shader of type " + shaderType + " failed. \n" << infoLog << std::endl;
		}
		return shader;
	}

	void LinkShaderProgram(GLuint vertexShader, GLuint fragmentShader)
	{
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertexShader);
		glAttachShader(this->Program, fragmentShader);
		glLinkProgram(this->Program);

		// Check for link errors
		GLint success;
		GLchar infoLog[512];
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "Error: Linking of shader program failed. \n" << infoLog << std::endl;
		}
	}

};

#endif