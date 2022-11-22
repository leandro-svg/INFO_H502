#include"EBO.h"
// Element buffer class

// Constructor that generates a Elements Buffer Object and links it to indices
EBO::EBO(GLuint* indices, GLsizeiptr size)
{
	// create the buffer object (one 3D object, reference pointing)
	glGenBuffers(1, &ID);
	// Binding = makes a certain object the current object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    // Need to specify following to improve the performence
    // GL_STREAM : vertices will be modified once and used a few times
    // GL_STATIC : vertices will be modified once and used a many times
    // GL_DYNAMIC : vertices will be modified multiple times and used a many times

    // DRAW : vertices will be modified and be used to draw an image on the screen
}

// Binds the EBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes the EBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}