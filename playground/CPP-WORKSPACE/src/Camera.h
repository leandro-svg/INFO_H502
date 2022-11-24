#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glad/glad.h>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/GLFW/glfw3.h>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm/glm.hpp>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm//gtc/matrix_transform.hpp>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm//gtc/type_ptr.hpp>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm//gtx/rotate_vector.hpp>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm//gtx/vector_angle.hpp>

#include "Shader.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 position;
	glm::vec3 orientationZ = glm::vec3(0.0f, 0.0f, -0.1f);
	glm::vec3 orientationY = glm::vec3(0.0f, 0.1f, 0.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.01f;
	float sensitivity = 10.0f;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
};
#endif