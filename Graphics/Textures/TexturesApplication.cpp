#include "TexturesApplication.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Textures::Textures()
{
	glfwInit();
	screen = glfwCreateWindow(1280, 720, "Rendering Geometry", nullptr, nullptr);

	if (screen == nullptr)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(screen);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(screen);
		glfwTerminate();
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	Gizmos::create();

	myCamera.setPerspective(PI * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	myCamera.setLookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));

	myCamera.setSpeed(10.0f);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1);
}

bool Textures::startup()
{
	return true;
}

bool Textures::update()
{
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCamera.update(deltaTime, screen);

	while (glfwWindowShouldClose(screen) == false && glfwGetKey(screen, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();
		return true;
	}
	return false;
}

void Textures::draw()
{
	Gizmos::draw(myCamera.getProjectionView());

	glfwSwapBuffers(screen);
	glfwPollEvents();
}

void Textures::shutdown()
{
	Gizmos::destroy();

	glfwDestroyWindow(screen);
	glfwTerminate();
}

std::string Textures::ReadIn(std::string fileName)
{
	std::string info;
	std::string container;
	std::ifstream file(fileName);

	if (file.is_open())
	{
		while (std::getline(file, info))
		{
			container += info + '\n';
		}
		file.close();
	}
	return container;
}