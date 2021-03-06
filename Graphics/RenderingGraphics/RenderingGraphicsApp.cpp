#include "RenderingGraphicsApp.h"


Renderer::Renderer()
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

//unsigned int indexCount;

bool Renderer::startup()
{
	generateSphere(25, 25, 5);

	//Vertex Shader
	const char* vsSource;
	std::string vertShader = ReadIn("vsSource.vert");
	vsSource = vertShader.c_str();

	//Fragment Shader
	const char* fsSource;
	std::string fragShader = ReadIn("fsSource.frag");
	fsSource = fragShader.c_str();

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//m_time = glfwGetTime();

	glUseProgram(m_programID);
	
	int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	//GLint loc = glGetUniformLocation(m_programID, "Time");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(myCamera.getProjectionView()));
	//glUniform1f(loc, m_time);

	//generateGrid(21, 21);
	//generatePlane();
	//generateCube();
	//generateHalfCircle(10, 10, 3);
	
	drawSphere();
	
	Gizmos::draw(myCamera.getProjectionView());

	glfwSwapBuffers(screen);
	glfwPollEvents();
}

void Renderer::shutdown()
{
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);

	Gizmos::destroy();

	glfwDestroyWindow(screen);
	glfwTerminate();
}

std::string Renderer::ReadIn(std::string fileName)
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

//function to create a grid
void Renderer::generateGrid(unsigned int rows, unsigned int cols)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Vertex* aoVertices = new Vertex[rows * cols];
	
	//Populates the vertices array
	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			aoVertices[r * cols + c].position = glm::vec4((float)c, 0, (float)r, 1);

			glm::vec3 colour = glm::vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = glm::vec4(colour, 1);
		}
	}

	unsigned int size = (rows - 1) * (cols - 1) * 6;
	auiIndices = new unsigned int[size];

	int index = 0;
	for (unsigned int r = 0; r < (rows - 1); r++)
	{
		for (unsigned int c = 0; c < (cols - 1); c++)
		{
			//Triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			//Triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
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
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}

void Renderer::generatePlane()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	Vertex aoPoints[4];
	unsigned int indices[4] = { 0, 1, 2, 3 };

	aoPoints[0].position = glm::vec4(4, 0, 4, 1);
	aoPoints[1].position = glm::vec4(-4, 0, 4, 1);
	aoPoints[2].position = glm::vec4(4, 0, -4, 1);
	aoPoints[3].position = glm::vec4(-4, 0, -4, 1);

	aoPoints[0].colour = glm::vec4(1, 0, 0, 1);
	aoPoints[1].colour = glm::vec4(0, 1, 0, 1);
	aoPoints[2].colour = glm::vec4(0, 0, 1, 1);
	aoPoints[3].colour = glm::vec4(1, 1, 1, 1);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), aoPoints, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::generateCube()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Vertex aoPoints[8];
	unsigned int indices[17] = { 0, 1, 2, 3, 6, 7, 4, 5, 0, 1, 5, 3, 7, 6, 4, 2, 0 };

	aoPoints[0].position = glm::vec4(4, 0, 4, 1);
	aoPoints[1].position = glm::vec4(-4, 0, 4, 1);
	aoPoints[2].position = glm::vec4(4, 0, -4, 1);
	aoPoints[3].position = glm::vec4(-4, 0, -4, 1);
	aoPoints[4].position = glm::vec4(4, 5, 4, 1);
	aoPoints[5].position = glm::vec4(-4, 5, 4, 1);
	aoPoints[6].position = glm::vec4(4, 5, -4, 1);
	aoPoints[7].position = glm::vec4(-4, 5, -4, 1);

	aoPoints[0].colour = glm::vec4(1, 0, 0, 1);
	aoPoints[1].colour = glm::vec4(0, 1, 0, 1);
	aoPoints[2].colour = glm::vec4(0, 0, 1, 1);
	aoPoints[3].colour = glm::vec4(1, 1, 1, 1);
	aoPoints[4].colour = glm::vec4(0.5f, 0, 0, 1);
	aoPoints[5].colour = glm::vec4(0, 0.5f, 0, 1);
	aoPoints[6].colour = glm::vec4(0, 0, 0.5f, 1);
	aoPoints[7].colour = glm::vec4(0.5f, 0.5f, 0.5f, 1);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), aoPoints, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 17 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::generateHalfCircle(unsigned int points, unsigned int segments, unsigned int radius)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(4.0f);

	Vertex* aoVertices = new Vertex[points * segments];

	for (unsigned int i = 0; i <= points; i++)
	{
		double angle = PI * i / segments;
		double X = cos(angle) * radius;
		double Y = sin(angle) * radius;
		
		aoVertices[i].position = glm::vec4(X, Y, 0, 1);
	}

	auiIndices = new unsigned int [2 * (points * segments)];
	unsigned int size = 2 * (points * segments);

	int count = 0;
	for (unsigned int i = 0; i < segments; i++)
	{
		for (unsigned int j = 0; j < points; j++)
		{
			auiIndices[count++] = i * points + j;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (points * segments) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, segments * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLE_FAN, size, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}

void Renderer::generateSphere(unsigned int pointCount, unsigned int partCount, float radius)
{
	unsigned int size = pointCount * partCount;
	Vertex* sphereVerts = new Vertex[size];
	unsigned int* sphereIndices;
	
	//Generates the vertices of a semicircle
	int sectionCount = pointCount - 1;
	Vertex* points = new Vertex[pointCount];

	for (unsigned int i = 0; i < pointCount; i++)
	{
		float theta = PI * (float)i / (float)sectionCount;
		float newX = radius * sin(theta);
		float newY = radius * cos(theta);

		points[i].position = glm::vec4(newX, newY, 0, 1);
	}

	//Generates the vertticies of the sphere based on those of the semicircle
	unsigned int counter = 0;
	Vertex* partVerts = new Vertex[pointCount * partCount];
	
	for (unsigned int i = 0; i < partCount; i++)
	{
		float phi = 2 * PI * ((float)i / (float)partCount);
		for (unsigned int j = 0; j < pointCount; j++, counter++)
		{
			float newX = points[j].position.x * cos(phi) + points[j].position.z * sin(phi);
			float newY = points[j].position.y;
			float newZ = points[j].position.z * cos(phi) - points[j].position.x * sin(phi);

			partVerts[counter].position = glm::vec4(newX, newY, newZ, 1);
		}
	}
	sphereVerts = partVerts;

	//Generates the indices of the sphere
	unsigned int* indices = new unsigned int[2 * (pointCount * (partCount))];
	indexCount = 2 * (pointCount * (partCount));

	for (unsigned int i = 0; i < partCount; i++)
	{
		unsigned int start = i * pointCount;
		for (unsigned int j = 0; j < pointCount; j++)
		{
			unsigned int botR = ((start + pointCount + j) % (pointCount * partCount));
			unsigned int botL = ((start + j) % (pointCount * partCount));
			indexHolder.push_back(botL);
			indexHolder.push_back(botR);
		}
	}

	for (unsigned int i = 0; i < indexHolder.size(); i++)
	{
		indices[i] = indexHolder[i];
	}
	sphereIndices = indices;

	
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), sphereVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indexCount * sizeof(unsigned int)), sphereIndices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::drawSphere()
{
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}