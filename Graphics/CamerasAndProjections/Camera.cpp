#include "Camera.h"

void Camera::setPerspective(float fieldOfView, float aspectRatio, float n, float f)
{
	projectionTransform = glm::perspective(fieldOfView, aspectRatio, n, f);
}

void Camera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up)
{
	viewTransform = glm::lookAt(from, to, up);
}

void Camera::setPosition(glm::vec3 position)
{
	worldTransform = worldTransform * glm::translate(position);
}

void Camera::rotateCamera(glm::mat4 rotate)
{
	projectionTransform = projectionTransform * rotate;
}

glm::mat4 Camera::getWorldTransform()
{
	return worldTransform;
}

glm::mat4 Camera::getView()
{
	return viewTransform;
}

glm::mat4 Camera::getProjection()
{
	return projectionTransform;
}


glm::mat4 Camera::getProjectionView()
{
	updateProjectionViewTransform();
	return projectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{
	projectionViewTransform = projectionTransform * viewTransform * worldTransform;
}

void FlyCamera::update(float deltaTime, GLFWwindow* window)
{
	glm::vec3 move = glm::vec3(0);
	glm::mat4 rotation = glm::mat4(1);

	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	double Xoffset, Yoffset;

	Xoffset = prevX - xPos;
	Yoffset = prevY - yPos;

	prevX = xPos;
	prevY = yPos;

	Xoffset *= 0.005;
	Yoffset *= 0.005;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		move.x += speed * deltaTime;
		move.z += speed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		move.x += speed * deltaTime;
		move.z -= speed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		move.x -= speed * deltaTime;
		move.z -= speed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		move.x -= speed * deltaTime;
		move.z += speed * deltaTime;
	}
	setPosition(move);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (Yoffset > 1 || Yoffset < 1)
		{
			float rotated = Yoffset;
			rotation[1][1] = cos(rotated);
			rotation[1][2] = -sin(rotated);
			rotation[2][1] = sin(rotated);
			rotation[2][2] = cos(rotated);
		}
		if (Xoffset > 1 || Xoffset < 1)
		{
			float rotated = Xoffset;
			rotation[0][0] = cos(rotated);
			rotation[0][2] = sin(rotated);
			rotation[2][0] = -sin(rotated);
			rotation[2][2] = cos(rotated);
		}
		rotateCamera(rotation);
	}

	getProjectionView();
}

void FlyCamera::setSpeed(float spd)
{
	speed = spd;
}
