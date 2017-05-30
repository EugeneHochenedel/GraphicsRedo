#pragma once

#define GLM_SWIZZLE
#define GLM_FORCE_PURE

#include <iostream>
#include "src\gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "src\Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>
#include <string>
#include <fstream>
#include "src\Camera.h"

struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
};

class Application
{
public:
	virtual bool startup() = 0;
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void shutdown() = 0;
};

class Renderer : public Application
{
public:
	Renderer();
	bool startup() override;
	bool update() override;
	void draw() override;
	void shutdown() override;

	void generateGrid(unsigned int rows, unsigned int cols);
	std::string ReadIn(std::string);
	void generatePlane();
	void generateCube();
	void generateHalfCircle(unsigned int points, unsigned int radius);

private:
	GLFWwindow* screen;

	FlyCamera myCamera;

	unsigned int m_programID;
	unsigned int *auiIndices;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	float previousTime, currentTime, deltaTime;

	glm::vec4 black, grey, red, brown, yellow, green, white;
};