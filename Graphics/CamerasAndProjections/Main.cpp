#include <iostream>
#include "src\gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "src\Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtx\transform.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec4;

int main() 
{
	float rotAngles = 0;

	vec4 green = vec4(0, 0.39f, 0, 1);
	vec4 red = vec4(1,0,0,1);
	vec4 brown = vec4(0.54f, 0.27f, 0.07f, 0.5f);
	vec4 grey = vec4(0.25f, 0.25f, 0.25f, 1);
	vec4 yellow = vec4(1, 1, 0, 1);

	if (glfwInit() == false)
	{
		return false;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Intro to OpenGL", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	Gizmos::create();

	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	mat4 projection = glm::ortho(-10.0f, 10.0f, 10.0f, -10.0f, 1.0f, 1000.0f);
	/*mat4 temp = glm::inverse(view);
	view[3].z = view[3].z * temp[3].z;
	
	float previousTime = 0.0f;
	float deltaTime = 0.0f;

	float currentTime = (float)glfwGetTime();
	previousTime = currentTime;
	float deltaTime = currentTime - previousTime;*/

	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Gizmos::clear();

		Gizmos::addTransform(mat4(1));

		rotAngles += 0.01f;

		mat4 Star = mat4(1) * glm::rotate(rotAngles, vec3(0, 1, 0));
		mat4 Exoplanet = Star * glm::translate(vec3(5, 0, 6)) * glm::rotate(72.0f, vec3(0, 1, 0));
		mat4 Satellite = (Exoplanet * glm::rotate(rotAngles * -5, vec3(0, 1, 0))) * glm::translate(vec3(1, 0, 0));

		vec4 white(1);
		vec4 black(0, 0, 0, 1);

		Gizmos::addSphere(vec3(Star[3]), 1, 20, 20, yellow, &Star);
		Gizmos::addSphere(vec3(Exoplanet[3]), 0.5f, 15, 15, green, &Exoplanet);
		Gizmos::addSphere(vec3(Satellite[3]), 0.25f, 10, 10, grey, &Satellite);

		for (int i = 0; i < 21; i++)
		{
			Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 0 ? white : black);
			Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 0 ? white : black);
		}

		Gizmos::draw(projection * view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Gizmos::destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}