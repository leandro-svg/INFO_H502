#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Camera::position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	// Initializes matrices 
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(position, position + orientationZ, orientationY);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}



void Camera::Inputs(GLFWwindow* window)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Zoom in
	{
		position += speed * orientationZ;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // Go left
	{
		position += speed/2 * -glm::normalize(glm::cross(orientationZ, orientationY))/2.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) // Zoom out
	{
		position += speed * -orientationZ;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // Go right
	{
		position += speed/2 * glm::normalize(glm::cross(orientationZ, orientationY))/2.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // Go up
	{
		position += speed * orientationY;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Go down
	{
		position += speed * -orientationY;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // Rotation left
	{
		orientationZ = glm::rotate(orientationZ, glm::radians(0.1f), orientationY);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // Rotation right
	{
		orientationZ = glm::rotate(orientationZ, glm::radians(0.1f), -orientationY);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // Rotation down
	{
		orientationZ = glm::rotate(orientationZ, glm::radians(0.1f), -orientationX);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // Rotation up
	{
		orientationZ = glm::rotate(orientationZ, glm::radians(0.1f), orientationX);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) // Faster
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) // Slower
	{
		speed = 0.1f;
	}
	
}