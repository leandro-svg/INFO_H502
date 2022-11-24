/// Shader object class ///

#ifndef SHADER_CLASS_H // Lets the c++ know not to open up the file twice
#define SHADER_CLASS_H

/// Libraries
#include </home/flo/INFO_H502/playground/CPP-WORKSPACE/include/glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor: Shader Program building from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);
	// Activation
	void Activate();
	// Delete
	void Delete();
private:
	// Shaders compilation check
	void compileErrors(unsigned int shader, const char* type);
};
#endif