// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#include <stdio.h>
#include <stdlib.h>
#include "Voxelizer_ABuffer.h"
#include "VoxelizerTools.h"

Voxelizer_ABufferData *voxelizerCreateABufferData(Voxelizer *voxelizer)
{
	GLfloat quadVerts[18] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};
	Voxelizer_ABufferData *data = (Voxelizer_ABufferData*) malloc(sizeof(Voxelizer_ABufferData));
	if (!data)
		voxelizerError("Failed to allocate Voxelizer_ABufferData", __LINE__);

	data->voxelizer = voxelizer;

	// shader programs
	data->fragCountProgram = voxelizerCreateAndLinkProgram("shaders/ModelTransform.vert", "shaders/FragCount.frag");
	data->makeFragListProgram = voxelizerCreateAndLinkProgram("shaders/ModelTransform.vert", "shaders/MakeFragList.frag");
	data->sortFragListProgram = voxelizerCreateAndLinkProgram("shaders/NoTransform.vert", "shaders/SortFragList.frag");

	// screen quad
	glGenBuffers(1, &(data->screenQuadBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, data->screenQuadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// counter
	voxelizerInitAtomicCounter(&(data->atomicCounter), 0);

	// shader storage buffer objects
	glGenBuffers(1, &(data->headPtrSSBO));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->headPtrSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelizer->width * voxelizer->height * sizeof(GLint), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, data->headPtrSSBO);

	glGenBuffers(1, &(data->nodePtrSSBO));
	glGenBuffers(1, &(data->nodeDataSSBO));

	return data;
}

void voxelizerDestroyABufferData(Voxelizer_ABufferData *data)
{
	GLuint buffers[] = {
		data->atomicCounter,
		data->headPtrSSBO,
		data->nodePtrSSBO,
		data->nodeDataSSBO
	};
	glDeleteBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);

	glDeleteProgram(data->fragCountProgram);
	glDeleteProgram(data->makeFragListProgram);
	glDeleteProgram(data->sortFragListProgram);

	free(data);
}

void voxelizerRunABuffer(Voxelizer *voxelizer)
{
	Voxelizer_ABufferData *data = (Voxelizer_ABufferData*)(voxelizer->data);
	GLfloat modelTransformMatrix[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	GLint modelTransformMatrixUniformLocation;
	GLint bufMask;
	GLint* headPtrs;
	int voxelGridSize = voxelizer->width * voxelizer->height;
	GLint framebufferWidthUniformLocation;
	int i;
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, voxelizer->framebuffer);
	glViewport(0, 0, voxelizer->width, voxelizer->height);


	// --- count fragments ---
	glBindBuffer(GL_ARRAY_BUFFER, voxelizer->meshVertBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	
	glUseProgram(data->fragCountProgram);

	voxelizerSetAtomicCounter(data->atomicCounter, 0);
	
	// make rotation matrix here
	
	modelTransformMatrixUniformLocation = glGetUniformLocation(data->fragCountProgram, "modelTransformMatrix");
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, modelTransformMatrix);

	glDrawArrays(GL_TRIANGLES, 0, voxelizer->meshVertCount);
	
	data->fragmentCount = voxelizerGetAtomicCounter(data->atomicCounter);


	// --- setup linked list memory ---
	bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->headPtrSSBO);
	headPtrs = (GLint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, voxelizer->width * voxelizer->height * sizeof(GLint), bufMask);
	for ( i = 0; i < voxelGridSize; i++ )
		headPtrs[i] = -1;
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->nodePtrSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->fragmentCount * sizeof(GLint), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, data->nodePtrSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->nodeDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->fragmentCount * sizeof(GLint), NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, data->nodeDataSSBO);


	// --- create linked list ---
	voxelizerSetAtomicCounter(data->atomicCounter, 0);

	glUseProgram(data->makeFragListProgram);

	framebufferWidthUniformLocation = glGetUniformLocation(data->makeFragListProgram, "framebufferWidth");
	glUniform1i(framebufferWidthUniformLocation, voxelizer->width);

	modelTransformMatrixUniformLocation = glGetUniformLocation(data->makeFragListProgram, "modelTransformMatrix");
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, modelTransformMatrix);
	
	glDrawArrays(GL_TRIANGLES, 0, voxelizer->meshVertCount);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// --- sort linked list ---
	glBindBuffer(GL_ARRAY_BUFFER, data->screenQuadBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glUseProgram(data->sortFragListProgram);

	framebufferWidthUniformLocation = glGetUniformLocation(data->sortFragListProgram, "framebufferWidth");
	glUniform1i(framebufferWidthUniformLocation, voxelizer->width);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

Voxelizer_ABufferLinkedList *voxelizerGetABufferLinkedList(Voxelizer *voxelizer)
{
	Voxelizer_ABufferData *data = (Voxelizer_ABufferData*)(voxelizer->data);
	Voxelizer_ABufferLinkedList *linkedList;
	
	VoxelizerDeviceContext saveDeviceContext = voxelizerGetCurrentDeviceContext();  // save caller's GL context
	VoxelizerContext saveContext = voxelizerGetCurrentContext();
	
	voxelizerMakeCurrent(voxelizerDeviceContext, voxelizerContext);  // use voxelizer's GL context

	linkedList = (Voxelizer_ABufferLinkedList * )malloc(sizeof(Voxelizer_ABufferLinkedList));

	if (!linkedList)
		voxelizerError("Failed to allocate Voxelizer_ABufferLinkedList", __LINE__);

	linkedList->width = voxelizer->width;
	linkedList->height = voxelizer->height;
	linkedList->depth = voxelizer->depth;

	linkedList->fragmentCount = data->fragmentCount;

	linkedList->headPointers = (GLint*) malloc(voxelizer->width * voxelizer->height * sizeof(GLint));
	if (!linkedList->headPointers)
		voxelizerError("Failed to allocate A-buffer linked list head pointers", __LINE__);

	linkedList->nodePointers = (GLint*) malloc(data->fragmentCount * sizeof(GLint));
	if (!linkedList->nodePointers)
		voxelizerError("Failed to allocate A-buffer linked list node pointers", __LINE__);

	linkedList->nodeData = (GLint*) malloc(data->fragmentCount * sizeof(GLint));
	if (!linkedList->nodeData)
		voxelizerError("Failed to allocate A-buffer linked list node data", __LINE__);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->headPtrSSBO);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, voxelizer->width * voxelizer->height * sizeof(GLint), linkedList->headPointers);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->nodePtrSSBO);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data->fragmentCount * sizeof(GLint), linkedList->nodePointers);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->nodeDataSSBO);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data->fragmentCount * sizeof(GLint), linkedList->nodeData);

	voxelizerMakeCurrent(saveDeviceContext, saveContext);  // restore caller's GL context

	return linkedList;
}

void voxelizerDestroyABufferLinkedList(Voxelizer_ABufferLinkedList *linkedList)
{
	free(linkedList);
}