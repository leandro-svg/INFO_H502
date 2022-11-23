#include"VBO.h"
// Vertex Buffer Object class 
// VBO to pack object with the vertex data

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{   
    // create the buffer object (one 3D object, reference pointing)
	glGenBuffers(1, &ID);
    // Binding = makes a certain object the current object
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    // Need to specify following to improve the performence
    // GL_STREAM : vertices will be modified once and used a few times
    // GL_STATIC : vertices will be modified once and used a many times
    // GL_DYNAMIC : vertices will be modified multiple times and used a many times

    // DRAW : vertices will be modified and be used to draw an image on the screen
}

// Binds the VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}