#include "Camera.h"
extern GLFWwindow* window;

Camera::Camera(float x, float y, float z)
{
	Position.x = x;
	Position.y = y;
	Position.z = z;
	Up = glm::vec3(0, 1, 0);
	Look = glm::vec3(0, 0, 0);

	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(
		Position, // Camera is at (4,3,3), in World Space
		Look, // and looks at the origin
		Up  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

Camera::Camera()
{
	Position = glm::vec3(0,0,0);
	Up = glm::vec3(0, 1, 0);
	Look = glm::vec3(0, 0, 0);
}

glm::mat4 Camera::GetProjection()
{
	return Projection;
}

glm::mat4 Camera::GetView()
{
	return View;
}

void Camera::CameraMove(float deltaTime)
{
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame (NOTE THE HARDCODED WINDOW SIZES!! BAD BAD BAD!!)
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
	// Camera matrix
	View = glm::lookAt(
		Position,           // Camera is here
		Position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
}