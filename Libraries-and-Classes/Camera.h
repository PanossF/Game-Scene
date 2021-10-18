#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Up;
	glm::vec3 Look;
	glm::mat4 Projection;
	glm::mat4 View;

	Camera();
	Camera(float x, float y, float z);


	glm::mat4 GetProjection();
	glm::mat4 GetView();

	void CameraMove(float deltaTime);

private:
	// Initial horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// Initial vertical angle : none
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 45.0f;

	float speed = 30.0f; // 3 units / second
	float mouseSpeed = 0.005f;

};