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
#include "Shader.h"
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
	// Tell GLFW what version of OpenGL we are using (OpenGL 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile (only have the modern functions)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "test", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Error, unable to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Set the window into the current context
	glfwMakeContextCurrent(window);
	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window (from x = 0, y = 0, to x = windowWidth, y = windowHeight)
	glViewport(0, 0, windowWidth, windowHeight);
	// Shader object creation using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	// Vertex Array object creation + binding (Binding = makes a certain object the current object)
	VAO VAO1;
	VAO1.Bind();
	// Vertex Buffer object creation + linking to the vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Element Buffer object creation + linking to the indices
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
	// Unbinding all
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	// Gets ID of uniform called "scale"
	GLuint uniformID = glGetUniformLocation(shaderProgram.ID, "scale");
	// Texture object creation
	Texture texture("/home/flo/INFO_H502/playground/CPP-WORKSPACE/rsrc/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	texture.texUnit(shaderProgram, "tex0", 0);
	// Enables the Depth Buffer (to not have only 2D view)
	glEnable(GL_DEPTH_TEST);
	// Camero object creation
	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Background color specification
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Back and Depth buffers cleaning
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Shader Program activation
		shaderProgram.Activate();
		// Set window as camera input
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");		
		// Texture binding
		texture.Bind();
		// VAO binding
		VAO1.Bind();
		// Primitives, number of indices, datatype of indices, index of indices drawing
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap between the back buffer and the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	texture.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}