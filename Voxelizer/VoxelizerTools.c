// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#include <stdio.h>
#include <stdlib.h>
#include "VoxelizerTools.h"

char* voxelizerReadFile(const char *fileName)
{
	FILE *input;
	long size;
	char *content;

	fopen_s(&input, fileName, "rb");
	if (input == NULL) 
		return NULL;
 
	if (fseek(input, 0, SEEK_END) == -1) 
	{ 
		fclose(input); 
		return NULL; 
	}

	size = ftell(input);
	if (size == -1) 
	{ 
		fclose(input); return NULL; 
	}
	if (fseek(input, 0, SEEK_SET) == -1) 
	{ 
		fclose(input); 
		return NULL; 
	}
 
	content = (char*) malloc( (size_t) size +1  ); 
	if (content == NULL) 
	{ 
		fclose(input); 
		return NULL; 
	}
 
	fread(content, 1, (size_t)size, input);
	if (ferror(input)) 
	{
		free(content);
		fclose(input);
		return NULL;
	}
 
	fclose(input);
	content[size] = '\0';

	return content;
}

GLuint voxelizerCreateShader(GLenum shaderType, const char *shaderFile)
{
	GLuint shader = glCreateShader(shaderType);
	const char* fileData = shaderFile;
	GLint status;
	GLint infoLogLength;
	GLchar *infoLog;
	const char *shaderTypeString = NULL;

	glShaderSource(shader, 1, &fileData, NULL);

	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		infoLog = (GLchar*) malloc(sizeof(GLchar) * (infoLogLength + 1));
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);

		switch (shaderType)
		{
		case GL_VERTEX_SHADER: shaderTypeString = "vertex"; break;
		case GL_FRAGMENT_SHADER: shaderTypeString = "fragment"; break;
		case GL_GEOMETRY_SHADER: shaderTypeString = "geometry"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", shaderTypeString, infoLog);
		free(infoLog);
	}

	return shader;
}

GLuint voxelizerCreateProgram(size_t shaderListSize, const GLuint *shaderList)
{
    GLuint program = glCreateProgram();
    size_t iLoop;
	GLint status;
	GLint infoLogLength;
	GLchar *strInfoLog;

    for (iLoop = 0; iLoop < shaderListSize; iLoop++)
    	glAttachShader(program, shaderList[iLoop]);
    
    glLinkProgram(program);
    
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        strInfoLog = (GLchar*) malloc(sizeof(GLchar) * (infoLogLength + 1));
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        free(strInfoLog);
    }
    
    for (iLoop = 0; iLoop < shaderListSize; iLoop++)
	{
        glDetachShader(program, shaderList[iLoop]);
		glDeleteShader(shaderList[iLoop]);
	}

    return program;
}

GLuint voxelizerCreateAndLinkProgram(const char *vertexFileName, const char *fragmentFileName)
{
	char *vertexShader = voxelizerReadFile(vertexFileName);
	char *fragmentShader = voxelizerReadFile(fragmentFileName);
	GLuint shaders[2];
	shaders[0] = voxelizerCreateShader(GL_VERTEX_SHADER, vertexShader);
	shaders[1] = voxelizerCreateShader(GL_FRAGMENT_SHADER, fragmentShader);

	return voxelizerCreateProgram(2, shaders);
}

void voxelizerInitAtomicCounter(GLuint *atomicCounter, GLint base)
{
	GLuint count = 0;
	glGenBuffers(1, atomicCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, *atomicCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &count, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, base, *atomicCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void voxelizerSetAtomicCounter(GLuint atomicCounter, GLuint value)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &value);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

GLuint voxelizerGetAtomicCounter(GLuint atomicCounter)
{
	GLuint count;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &count);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	return count;
}