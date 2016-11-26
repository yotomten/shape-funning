#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float deltaTime;

void main()
{
	vec3 pos = position;
	pos.x = (1 + sin(deltaTime)) * pos.x;
	pos.y = (1 + cos(deltaTime)) * pos.y;
    gl_Position = proj * view * model * vec4(position, 1.0f);
    TexCoords = texCoords;
}