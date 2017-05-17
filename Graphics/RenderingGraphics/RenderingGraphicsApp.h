#pragma once

#define GLM_SWIZZLE
#define GLM_FORCE_PURE

#include <iostream>
//#include <math.h>
//#include <vector>
#include "src\gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "src\Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>
//#include <string>
//#include <fstream>
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

	void generateGrid(const int rows, const int cols);

private:
	GLFWwindow* screen;
	glm::mat4 view, projection, m_projectionViewMatrix;
	FlyCamera myCamera;

	unsigned int m_programID;
	int auiIndices;

	unsigned int m_VAO, m_VBO, m_IBO;

	float previousTime, currentTime, deltaTime;

	glm::vec4 black, grey, red, brown, yellow, green, white;
};