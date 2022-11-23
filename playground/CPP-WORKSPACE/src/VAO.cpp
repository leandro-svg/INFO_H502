#include"VAO.h"
// Vertex array object class

// VAO to points to one or more VBOs and tells OpenGL how to interpret them
// allows to quickly switch between different VBOs

//ATTENTION: Need to generate the VAO before the VBO

// Constructor that generates a VAO ID
VAO::VAO()
{
	// create the buffer object (one 3D object, reference pointing)
	glGenVertexArrays(1, &ID);
}

// Links a VBO Attribute such as a position or color to the VAO
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	// Need configuration such that opengl knows how to read the VBO
	// (index vertex attribute, # values per vertex, types of values, if coordinate in int, # data between each vertex, pointer where the vertices begin in the array)
	// index vertex attribute allows to communicate with vertex shader from the outside
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

// Binds the VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}

// Unbinds the VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Deletes the VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}