// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#pragma once
#ifndef H_VOXELIZERTOOLS
#define H_VOXELIZERTOOLS

#include <GL\glew.h>

char* voxelizerReadFile(const char *fileName);

GLuint voxelizerCreateShader(GLenum shaderType, const char *shaderFile);
GLuint voxelizerCreateProgram(size_t shaderListSize, const GLuint *shaderList);
GLuint voxelizerCreateAndLinkProgram(const char *vertexFileName, const char *fragmentFileName);

void voxelizerInitAtomicCounter(GLuint *atomicCounter, GLint base);
void voxelizerSetAtomicCounter(GLuint atomicCounter, GLuint value);
GLuint voxelizerGetAtomicCounter(GLuint atomicCounter);

#endif