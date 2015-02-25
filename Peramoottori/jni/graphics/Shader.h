#ifndef SHADER_H
#define SHADER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

class Shader
{
public:

	Shader();
	Shader(GLuint shader);
	~Shader();

	Shader LoadShader(const char* filePath, GLenum ShaderType);

	//unsigned int AddShader(Shader shader, GLuint program);

private:

	GLuint shader;
	GLint compiled;

};

#endif