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
	glm::vec2 textureCoords;
	glm::vec4 normal;
	glm::vec4 tangent;
};

class Application
{
public:
	virtual bool startup() = 0;
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void shutdown() = 0;
};

class Textures : public Application
{
public:
	Textures();
	bool startup() override;
	bool update() override;
	void draw() override;
	void shutdown() override;
	
	std::string ReadIn(std::string);
	bool linkShaders(std::string, std::string);
	void generateBasicTexturePlane();
	void generateAdvanceTexturePlane();

private:
	GLFWwindow* screen;
	FlyCamera myCamera;

	unsigned int m_programID;

	unsigned int m_VAO, m_VBO, m_IBO, m_Textures, m_NormalMap;

	float previousTime, currentTime, deltaTime, m_time;
	float PI = glm::pi<float>();
};