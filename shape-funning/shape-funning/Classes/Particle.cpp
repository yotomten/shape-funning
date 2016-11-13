#include "Particle.h"


Particle::Particle(GLfloat mass, GLfloat x, GLfloat y, GLfloat z, GLfloat s, GLfloat t)
{
	this->Position[0] = x;
	this->Position[1] = y;
	this->Position[2] = z;

	this->TextureCoordinates[0] = s;
	this->TextureCoordinates[1] = t;

	this->mass = mass;

}


Particle::~Particle()
{
}
