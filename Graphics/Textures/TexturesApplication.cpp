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
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;
	
	data = stbi_load("./data/textures/crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_Textures);

	glBindTexture(GL_TEXTURE_2D, m_Textures);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	/*data = stbi_load("./data/textures/rock_normal.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_NormalMap);

	glBindTexture(GL_TEXTURE_2D, m_NormalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);*/


	linkShaders("tvsSource.vert", "tfsSource.frag");

	generateBasicTexturePlane();
	//generateAdvanceTexturePlane();

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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(myCamera.getProjectionView()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Textures);

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalMap);*/

	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);

	/*loc = glGetUniformLocation(m_programID, "normal");
	glUniform1i(loc, 1);

	glm::vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));
	loc = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(loc, light.x, light.y, light.z);*/

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, nullptr);

	Gizmos::draw(myCamera.getProjectionView());

	glfwSwapBuffers(screen);
	glfwPollEvents();
}

void Textures::shutdown()
{
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);

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

bool Textures::linkShaders(std::string vs, std::string fs)
{
	std::string vertShader = ReadIn(vs);
	const char* vsSource = vertShader.c_str();

	std::string fragShader = ReadIn(fs);
	const char* fsSource = fragShader.c_str();

	int success = GL_FALSE;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;

		glfwDestroyWindow(screen);
		glfwTerminate();
		return false;
	}
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void Textures::generateBasicTexturePlane()
{
	float vertexData[] = { 
			//Position, TexCoord, Colour
			-5, 0, 5, 1, 0, 1, 1, 1, 1,
			5, 0, 5, 1, 2, 1, 1, 0, 0,
			5, 0, -5, 1, 2, 0, 0, 1, 0,
			-5, 0, -5, 1, 0, 0, 0, 0, 1,
	};

	unsigned int indexData[] = { 
			0, 1, 2,
			0, 2, 3, 
	};
	
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * 4, &vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &indexData, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(4 * sizeof(float)));

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Textures::generateAdvanceTexturePlane()
{
	Vertex verts[] = {
		//position, texturecoords, normal, tangent
		{glm::vec4(-5, 0, 5, 1), glm::vec2(0, 1), glm::vec4(0, 1, 0, 0), glm::vec4(1, 0, 0, 0)},
		{glm::vec4(5, 0, 5, 1), glm::vec2(2, 1), glm::vec4(0, 1, 0, 0), glm::vec4(1, 0, 0, 0)},
		{glm::vec4(5, 0, -5, 1), glm::vec2(2, 0), glm::vec4(0, 1, 0, 0), glm::vec4(1, 0, 0, 0)},
		{glm::vec4(-5, 0, -5, 1), glm::vec2(0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(1, 0, 0, 0)},
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex) * 9, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + 48);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + 16);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + 32);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}