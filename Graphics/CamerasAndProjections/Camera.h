#pragma once

#define GLM_SWIZZLE
#define GLM_FORCE_PURE

#include <iostream>
#include "src\gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "src\Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtx\transform.hpp>

class Camera
{
public:
	virtual void update(float deltaTime) = 0;
	void setPerspective(float fieldOfView, float aspectRatio, float near, float far);
	void setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);
	void setPosition(glm::vec3 position);
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
	void update(float deltaTime) override;
	void setSpeed(float speed);

private:
	float speed;
	glm::vec3 up;
};