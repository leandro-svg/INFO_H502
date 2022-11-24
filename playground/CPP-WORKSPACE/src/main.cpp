/// Libraries
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glad/glad.h>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/GLFW/glfw3.h>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/stb/stb_image.h>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm/glm.hpp>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm/gtc/matrix_transform.hpp>
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <tgmath.h>

/// Headers
#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

// Constants
const unsigned int windowWidth = 800;
const unsigned int windowHeight = 800;

// Vertices coordinates (origins of x and y located at the middle of the window)
GLfloat vertices[] =
{ //     Coordinates     /        Colors         /   Texture Coordinates
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	0.5f, 0.0f, -0.5f,      0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	0.5f, 0.0f,  0.5f,      0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 
	
	-0.5f, 1.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 5.0f,
	-0.5f, 1.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 5.0f,
	 0.5f, 1.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 5.0f,
	 0.5f, 1.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 5.0f

};

// Indices to order the vertices
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	4, 7, 6,
	4, 6, 5,
	0, 1, 5,
	0, 5, 4,
	2, 3, 7,
	2, 7, 6,
	1, 2, 6,
	1, 6, 5,
	3, 0, 4,
	3, 4, 7,


};


int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "test", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Set the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, windowWidth, windowHeight);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
    //   First Vertex  | Second Vertex
    // X Y Z  R  G  B  | X  Y  Z  R  G  B 
    //0 4 8 12 16 20 24   28 32 36 40 44
    // Coord Stride    |
    //Col oft| Color stride      |
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Texture
	Texture brickTex("/home/flo/INFO_H502/playground/CPP-WORKSPACE/rsrc/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);

	// Variables to obtain a rotation
	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	// Enables the Depth Buffer (to not have only 2D)
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");		
		// Binds texture so that is appears in rendering
		brickTex.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brickTex.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}