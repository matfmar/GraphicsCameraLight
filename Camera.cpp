//klasa obs³uguj¹ca kamerê
#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;		//lewo-prawo (obrót)
	pitch = startPitch;	//góra-dó³ (obrót)
	roll = 0.0f;	// ostatni wariant obrotu
	zoomAngle = 45.0f;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	
	up = worldUp;
	right = glm::cross(front, up);
	front_const = glm::normalize(front);	//kamerê definiuje po³o¿enie oraz 3 wektory - kierunek patrzenia(front) oraz 2 prostopad³e: up i right
	up_const = glm::normalize(up);
	right_const = glm::normalize(right);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;
	zoomSpeed = 1.0f;
	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime) {
	GLfloat velocity = moveSpeed * deltaTime;
	GLfloat velocityTurns = turnSpeed * deltaTime;
	GLfloat velocityRoll = turnSpeed * deltaTime;
	GLfloat velocityZoom = zoomSpeed * deltaTime * 20.0;

	if (keys[GLFW_KEY_W]) {
		position += front * velocity;
	}
	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}
	if (keys[GLFW_KEY_A]) {
		position -= right * velocity;
	}
	if (keys[GLFW_KEY_D]) {
		position += right * velocity;
	}
	if (keys[GLFW_KEY_H]) {
		position += up * velocity;
	}
	if (keys[GLFW_KEY_L]) {
		position -= up * velocity;
	}
	if (keys[GLFW_KEY_G]) {
		zoomAngle -= velocityZoom;
		if (zoomAngle <= 1.0f)
			zoomAngle = 1.0f;
	}
	if (keys[GLFW_KEY_F]) {
		zoomAngle += velocityZoom;
		if (zoomAngle >= 170.0f)
			zoomAngle = 170.0f;
	}
	if (keys[GLFW_KEY_Z]) {
		roll -= velocityRoll;
		if (roll < -89.0f)
			roll = -89.0f;
		update();
	}
	if (keys[GLFW_KEY_C]) {
		roll += velocityRoll;
		if (roll > 89.0f)
			roll = 89.0f;
		update();
	}
	if (keys[GLFW_KEY_LEFT]) {
		yaw -= velocityTurns;
		update();
	}
	if (keys[GLFW_KEY_RIGHT]) {
		yaw += velocityTurns;
		update();
	}
	if (keys[GLFW_KEY_DOWN]) {
		pitch -= velocityTurns;
		if (pitch < -89.0f)
			pitch = -89.0f;
		update();
	}
	if (keys[GLFW_KEY_UP]) {
		pitch += velocityTurns;
		if (pitch > 89.0f)
			pitch = 89.0f;
		update();
	}
	if (keys[GLFW_KEY_R]) {
		pitch = 0.0f;
		roll = 0.0f;
		yaw = 0.0f;
		zoomAngle = 45.0f;
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		update();
	}
}
//ta funkcja tworzy macierz perspektywy w oparciu o wzory z literatury
glm::mat4 perspective(GLfloat angle, GLfloat aspect, GLfloat near, GLfloat far) {
	glm::mat4 result = glm::mat4(1.0f);
	result = {
		1 / (aspect * tan(glm::radians(angle) / 2)), 0, 0, 0,
		0, 1 / tan(glm::radians(angle) / 2), 0, 0,
		0, 0, -((far + near) / (far - near)), -(2 * far * near / (far - near)),
		0, 0, -1, 0
	};
	result = glm::transpose(result);	//czasem transponujê macierze, poniewa¿ openGL czasami ma inn¹ konwencjê wiersz-kolumna ni¿ ksi¹¿ki
	return result;
}
//ta funkcja zmienia macierz perspektywy o k¹t, czyli robi zoom
glm::mat4 Camera::zoom(GLfloat w, GLfloat h, GLfloat n, GLfloat f) {
	glm::mat4 result = perspective(zoomAngle, w/h, n, f);
	return result;
}
//ta funkcja tworzy macierz widoku, która transformuje wspó³rzêdne œwiata na wspó³rzêdne w uk³adzie kamery - stworzona w oparciu o wzory z literatury
glm::mat4 lookAtPoint(glm::vec3 eye, glm::vec3 center, glm::vec3 upv)
{
	glm::vec3  f = glm::normalize(center - eye);
	glm::vec3  u = glm::normalize(upv);
	glm::vec3  s = glm::normalize(glm::cross(f, u));
	u = glm::cross(s, f);

	glm::mat4 Result(1);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -glm::dot(s, eye);
	Result[3][1] = -glm::dot(u, eye);
	Result[3][2] = glm::dot(f, eye);
	return Result;
}

glm::mat4 Camera::calculateViewMatrix() {
	return lookAtPoint(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition() {
	return position;
}
//obroty to w rzeczywistoœci obrót wektorów kamery wg pewnych osi - poni¿sze stworzone w oparciu o wzory macierzy na obroty
void Camera::update() {
	glm::mat3 turnPitch = {
		1, 0, 0,
		0, cos(glm::radians(pitch)), -sin(glm::radians(pitch)),
		0, sin(glm::radians(pitch)), cos(glm::radians(pitch))
	};
	glm::mat3 turnYaw = {
		cos(glm::radians(yaw)), 0, sin(glm::radians(yaw)),
		0, 1, 0,
		-sin(glm::radians(yaw)), 0, cos(glm::radians(yaw))
	};
	glm::mat3 turnRoll = {
		cos(glm::radians(roll)), -sin(glm::radians(roll)), 0,
		sin(glm::radians(roll)), cos(glm::radians(roll)), 0,
		0, 0, 1
	};
	glm::mat3 total = turnRoll * turnYaw * turnPitch;
	front = total * front_const;
	right = total * right_const;
	up = total * up_const;
	front = glm::normalize(front);
	right = glm::normalize(right);
	up = glm::normalize(up);
}

Camera::~Camera() {}