#pragma once

#define GLM_SWIZZLE
#define GLM_FORCE_PURE

#include <iostream>
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtx\transform.hpp>

class Camera
{
public:
	virtual void update(float deltaTime, GLFWwindow* window) = 0;
	void setPerspective(float fieldOfView, float aspectRatio, float near, float far);
	void setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);
	void setPosition(glm::vec3 position);
	void rotateCamera(glm::mat4 rotate);
	glm::mat4 getWorldTransform();
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getProjectionView();

private:
	void updateProjectionViewTransform();
	glm::mat4 worldTransform;
	glm::mat4 viewTransform;
	glm::mat4 projectionTransform;
	glm::mat4 projectionViewTransform;
};

class FlyCamera : public Camera
{
public:
	void update(float deltaTime, GLFWwindow* window) override;
	void setSpeed(float spd);

private:
	float speed;
	double prevX = 0, prevY = 0;
	glm::vec3 up;
};