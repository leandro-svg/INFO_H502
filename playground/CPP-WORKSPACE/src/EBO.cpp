/// Element buffer object class ///
// Index buffer

/// Headers
#include"EBO.h"

// Constructor: Elements Buffer object generation + indices linking
EBO::EBO(GLuint* indices, GLsizeiptr size)
{
	// Buffer object generation (one 3D object, reference pointing)
	glGenBuffers(1, &ID);
	// Binding 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    // Need to specify following to improve the performence
    // GL_STREAM : vertices will be modified once and used a few times
    // GL_STATIC : vertices will be modified once and used a many times
    // GL_DYNAMIC : vertices will be modified multiple times and used a many times

    // DRAW : vertices will be modified and be used to draw an image on the screen
}

// Binding
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinding
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Delete
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}