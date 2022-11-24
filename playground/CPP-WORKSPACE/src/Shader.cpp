/// Shader object class ///

/// Headers
#include"Shader.h"

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

// Constructor: Shader Program building from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// VertexFile and FragmentFile reading + storage of the strings
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	// Shader source strings convertion into character arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

// VERTEX SHADER
	// Vertex Shader Object creation + reference creation
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Vertex Shader source attachment to the Vertex Shader Object
	// (reference value, 1 string for the whole shader, point to the source code, NULL)
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Vertex Shader compilation into machine code
	glCompileShader(vertexShader);
// FRAGMENT SHADER
	// Fragment Shader Object creation + reference creation
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Fragment Shader source attachment to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Fragment Shader compilation into machine code
	glCompileShader(fragmentShader);

// Need to wrap both shaders up into a Shader Program
	// Shader Program Object creation + reference creation
	ID = glCreateProgram();
	// Vertex and Fragment Shaders attachment to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);
	// Delete the Vertex and Fragment Shader objects (now useless)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// Activation
void Shader::Activate()
{
	glUseProgram(ID);
}

// Delete
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// Shaders compilation check
void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}