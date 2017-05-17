#include "RenderingGraphicsApp.h"


Renderer::Renderer()
{
	/*glfwInit();
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

	myCamera.setPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	myCamera.setLookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));

	myCamera.setSpeed(10.0f);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1);*/
}

bool Renderer::startup()
{
	black = glm::vec4(glm::vec3(0), 1);
	grey = glm::vec4(glm::vec3(0.25f), 1);
	red = glm::vec4(1, 0, 0, 1);
	brown = glm::vec4(0.54f, 0.27f, 0.07f, 0.5f);
	yellow = glm::vec4(1, 1, 0, 1);
	green = glm::vec4(0, 1, 0, 1);
	white = glm::vec4(1);

	if (!glfwInit())
	{
		return false;
	}

	screen = glfwCreateWindow(1280, 720, "Rendering Graphics", nullptr, nullptr);

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

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	Gizmos::create();

	myCamera.setPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
	myCamera.setLookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));

	myCamera.setSpeed(10.0f);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1);

	//Vertex Shader
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							out vec4 vColour; \
							uniform mat4 ProjectionView; \
							uniform float time; \
							uniform float heightScale; \
							void main() { vec4 P = Position; vColour = Colour; P.y += sin( time * Position.x ) * heightScale; gl_Position = ProjectionView * P; }";

	//Fragment Shader
	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 FragColour; \
							void main() { FragColour = vColour; }";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	//Compiles the shaders
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
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

	generateGrid(21, 21);

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return true;
}

bool Renderer::update()
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

void Renderer::draw()
{
	/*for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), i == 0 ? white : black);
		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), i == 0 ? white : black);
	}*/

	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(myCamera.getProjectionView()));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, auiIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	Gizmos::draw(myCamera.getProjectionView());

	glfwSwapBuffers(screen);
	glfwPollEvents();
}

void Renderer::shutdown()
{
	Gizmos::destroy();

	glfwDestroyWindow(screen);
	glfwTerminate();
}

//function to create a grid
void Renderer::generateGrid(const int rows, const int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];
	
	
	//Populates the vertices array
	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			aoVertices[r * cols + c].position = glm::vec4((float)c, 0, (float)r, 1);

			//Create some arbitrary colour based off something
			//That might not be related to tiling a texture
			glm::vec3 colour = glm::vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r*cols + c].colour = glm::vec4(colour, 1);
		}
	}

	/*unsigned int**/ auiIndices = (rows - 1) * (cols - 1) * 6;
	unsigned int* aIndices = new unsigned int[auiIndices];

	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); r++)
	{
		for (unsigned int c = 0; c < (cols - 1); c++)
		{
			//Triangle 1
			aIndices[index++] = r * cols + c;
			aIndices[index++] = (r + 1) * cols + c;
			aIndices[index++] = (r + 1) * cols + (c + 1);

			//Triangle 2
			aIndices[index++] = r * cols + c;
			aIndices[index++] = (r + 1) * cols + (c + 1);
			aIndices[index++] = r * cols + (c + 1);
		}
	}
	//Generate our GL Buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	//Generate a Vertex Array Object
	glGenVertexArrays(1, &m_VAO);
	
	glBindVertexArray(m_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, auiIndices * sizeof(unsigned int), aIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDrawElements(GL_TRIANGLES, auiIndices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] aIndices;
}
