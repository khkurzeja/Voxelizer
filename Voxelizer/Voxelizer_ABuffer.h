// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#pragma once
#ifndef H_VOXELIZER_ABUFFER
#define H_VOXELIZER_ABUFFER

#include "Voxelizer.h"

typedef struct Voxelizer_ABufferLinkedList
{
	int width;
	int height;
	int depth;

	int fragmentCount;
	int *headPointers;
	int *nodePointers;
	int *nodeData;

} Voxelizer_ABufferLinkedList;

typedef struct Voxelizer_ABufferData
{
	Voxelizer *voxelizer;

	GLuint fragCountProgram;
	GLuint makeFragListProgram;
	GLuint sortFragListProgram;

	GLuint screenQuadBuffer;

	GLuint fragmentCount;
	GLuint atomicCounter;
	GLuint headPtrSSBO;
	GLuint nodePtrSSBO;
	GLuint nodeDataSSBO;

} Voxelizer_ABufferData;


Voxelizer_ABufferData *voxelizerCreateABufferData(Voxelizer *voxelizer);
void voxelizerDestroyABufferData(Voxelizer_ABufferData *data);

void voxelizerRunABuffer(Voxelizer *voxelizer);

Voxelizer_ABufferLinkedList *voxelizerGetABufferLinkedList(Voxelizer *voxelizer);
void voxelizerDestroyABufferLinkedList(Voxelizer_ABufferLinkedList *linkedList);

#endif