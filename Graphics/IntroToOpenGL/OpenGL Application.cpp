#include "OpenGL Application.h"

OpenGLApplication::OpenGLApplication()
{
	rotAngle = 0;

	black = glm::vec4(glm::vec3(0), 1);
	grey = glm::vec4(glm::vec3(0.25f), 1);
	red = glm::vec4(1, 0, 0, 1);
	brown = glm::vec4(0.54f, 0.27f, 0.07f, 0.5f);
	yellow = glm::vec4(1, 1, 0, 1);
	green = glm::vec4(0, 1, 0, 1);
	white = glm::vec4(1);
}

bool OpenGLApplication::startup()
{
	if (!glfwInit()) 
	{
		return false;
	}

	screen = glfwCreateWindow(1280, 720, "Intro to OpenGL", nullptr, nullptr);

	if (screen == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(screen);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(screen);
		glfwTerminate();
		return false;
	}

	view = glm::lookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));
	projection = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	
	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	Gizmos::create();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1);

	return true;
}

bool OpenGLApplication::update()
{
	while (glfwWindowShouldClose(screen) == false && glfwGetKey(screen, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Gizmos::clear();

		rotAngle += 0.01f;

		Star = glm::mat4(1) * glm::rotate(rotAngle, glm::vec3(0, 0.25f, 0));
		Exoplanet = (Star * glm::rotate(72.0f, glm::vec3(0, 1, 0))) * glm::translate(glm::vec3(2.5f, 0, 3));

		Satellite = glm::rotate(Exoplanet, rotAngle * -5.0f, glm::vec3(0, -1, 0.04f)) * glm::translate(glm::vec3(1.5f, 0, 0));

		Rings = glm::rotate(Satellite, 32.0f, glm::vec3(0, 0, 1));

		return true;
	}
	return false;
}

void OpenGLApplication::draw()
{
	Gizmos::addSphere(glm::vec3(Star[3]), 1, 20, 20, yellow, &Star);
	Gizmos::addSphere(glm::vec3(Exoplanet[3]), 0.5f, 15, 15, green, &Exoplanet);
	Gizmos::addSphere(glm::vec3(Satellite[3]), 0.25f, 10, 10, grey, &Satellite);
	Gizmos::addRing(glm::vec3(Satellite[3]), 1.0f, 0.5f, 10, brown, &Rings);

	for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), i == 0 ? white : black);
		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), i == 0 ? white : black);
	}

	Gizmos::draw(projection * view);

	glfwSwapBuffers(screen);
	glfwPollEvents();
}

void OpenGLApplication::shutdown()
{
	Gizmos::destroy();

	glfwDestroyWindow(screen);
	glfwTerminate();
}