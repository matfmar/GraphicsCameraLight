#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	void keyControl(bool* keys, GLfloat deltaTime);
	glm::mat4 calculateViewMatrix();
	glm::mat4 zoom(GLfloat w, GLfloat h, GLfloat n, GLfloat f);
	glm::vec3 getCameraPosition();
	~Camera();
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	glm::vec3 front_const;
	glm::vec3 up_const;
	glm::vec3 right_const;

	GLfloat yaw;
	GLfloat pitch;
	GLfloat roll;
	GLfloat zoomAngle;
	GLfloat moveSpeed;
	GLfloat turnSpeed;
	GLfloat zoomSpeed;
	void update();
};

