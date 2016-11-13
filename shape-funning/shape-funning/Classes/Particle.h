#pragma once

#include <GLFW/glfw3.h>

class Particle
{
public:

	GLfloat Position[3];
	GLfloat TextureCoordinates[2];
	GLfloat mass;

	Particle(GLfloat mass, GLfloat x, GLfloat y, GLfloat z, GLfloat s, GLfloat t);
	~Particle();

};

