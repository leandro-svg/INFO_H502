#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glad/glad.h>

// The VBO is an array of references
class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif