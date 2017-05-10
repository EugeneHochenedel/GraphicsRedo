#pragma once

#define GLM_SWIZZLE
#define GLM_FORCE_PURE

#include <iostream>
#include "src\gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "src\Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>

class Application
{
public:
	virtual bool startup() = 0;
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void shutdown() = 0;
};

class OpenGLApplication : public Application 
{
public:
	OpenGLApplication();
	bool startup() override;
	bool update() override;
	void draw() override;
	void shutdown() override;

private:
	GLFWwindow* screen;
	glm::mat4 view, projection, Star, Exoplanet, Satellite, Rings, inverted;

	float rotAngle;
	glm::vec4 black, grey, red, brown, yellow, green, white;
};