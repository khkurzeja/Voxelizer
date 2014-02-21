// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#pragma once
#ifndef H_VOXELIZER
#define H_VOXELIZER

#include <GL\glew.h>

#if _WIN32
	#include "Voxelizer_Windows.h"
#endif

typedef enum {VOXELIZER_ABUFFER} VoxelizerAlgorithmEnum;

typedef struct Voxelizer
{
	VoxelizerAlgorithmEnum algorithm;

	int width;
	int height;
	int depth;

	GLuint framebuffer;
	GLuint framebufferColor;

	unsigned int meshVertCount;
	float *meshVertPositions;
	GLuint meshVertBuffer;

	void *data;  // point to algorithm specific struct

} Voxelizer;

extern int voxelizerCount;
extern VoxelizerWindow voxelizerWindow;
extern VoxelizerDeviceContext voxelizerDeviceContext;
extern VoxelizerContext voxelizerContext;

extern void voxelizerError(const char *output, int line);

Voxelizer *voxelizerCreate(VoxelizerAlgorithmEnum algorithm, int width, int height, int depth);
void voxelizerDestroy(Voxelizer *voxelizer);

void voxelizerSetMesh(Voxelizer *voxelizer, unsigned int meshVertCount, float *meshVertPositions);

void voxelizerRun(Voxelizer *voxelizer);

#endif