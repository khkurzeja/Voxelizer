// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#include <stdio.h>
#include <stdlib.h>
#include "Voxelizer.h"
#include "Voxelizer_ABuffer.h"

int voxelizerCount = 0;
VoxelizerWindow voxelizerWindow;
VoxelizerDeviceContext voxelizerDeviceContext;
VoxelizerContext voxelizerContext;

void voxelizerError(const char *output, int line)
{
	if (line == -1)
		printf("Error: %s\n", output);
	else
		printf("Error on line %d: %s\n", line, output);
	exit(0);
}

Voxelizer *voxelizerCreate(VoxelizerAlgorithmEnum algorithm, int width, int height, int depth)
{
	VoxelizerDeviceContext saveDeviceContext;
	VoxelizerContext saveContext;
	Voxelizer *voxelizer = (Voxelizer*)malloc( sizeof(Voxelizer) );
	if (!voxelizer)
		voxelizerError("Failed to allocate Voxelizer", __LINE__);

	saveDeviceContext = voxelizerGetCurrentDeviceContext();  // save caller's GL context
	saveContext = voxelizerGetCurrentContext();

	if (voxelizerCount == 0)
	{
		voxelizerWindow = voxelizerCreateWindow();
		voxelizerInitContext(voxelizerWindow, &voxelizerDeviceContext, &voxelizerContext);  // create GL context
		voxelizerMakeCurrent(voxelizerDeviceContext, voxelizerContext);  // use voxelizer's GL context to init glew
		glewInit();
	}
	voxelizerCount++;

	voxelizerMakeCurrent(voxelizerDeviceContext, voxelizerContext);  // use voxelizer's GL context

	voxelizer->width = width;
	voxelizer->height = height;
	voxelizer->depth = depth;

	// create framebuffer
	glGenFramebuffers(1, &(voxelizer->framebuffer));
	glBindFramebuffer(GL_FRAMEBUFFER, voxelizer->framebuffer);
	
	glGenTextures(1, &(voxelizer->framebufferColor));
	glBindTexture(GL_TEXTURE_RECTANGLE, voxelizer->framebufferColor);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, voxelizer->framebufferColor, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// create algorithm data
	voxelizer->algorithm = algorithm;
	switch (algorithm)
	{
	case VOXELIZER_ABUFFER:
		voxelizer->data = voxelizerCreateABufferData(voxelizer);
		break;
	}

	voxelizerMakeCurrent(saveDeviceContext, saveContext);  // restore caller's GL context

	return voxelizer;
}

void voxelizerDestroy(Voxelizer *voxelizer)
{
	VoxelizerDeviceContext saveDeviceContext = voxelizerGetCurrentDeviceContext();  // save caller's GL context
	VoxelizerContext saveContext = voxelizerGetCurrentContext();

	voxelizerMakeCurrent(voxelizerDeviceContext, voxelizerContext);  // use voxelizer's GL context

	glDeleteTextures(1, &(voxelizer->framebufferColor));
	glDeleteBuffers(1, &(voxelizer->framebuffer));
	glDeleteBuffers(1, &(voxelizer->meshVertBuffer));

	switch (voxelizer->algorithm)
	{
	case VOXELIZER_ABUFFER:
		voxelizerDestroyABufferData((Voxelizer_ABufferData*)(voxelizer->data));
		break;
	}

	voxelizerCount--;
	if (voxelizerCount == 0)
	{
		voxelizerDestroyContext(voxelizerWindow, voxelizerDeviceContext, voxelizerContext);
		voxelizerDestroyWindow(voxelizerWindow);
	}
	
	free(voxelizer);

	voxelizerMakeCurrent(saveDeviceContext, saveContext);  // restore caller's GL context
}

void voxelizerSetMesh(Voxelizer *voxelizer, unsigned int meshVertCount, float *meshVertPositions)
{
	VoxelizerDeviceContext saveDeviceContext = voxelizerGetCurrentDeviceContext();  // save caller's GL context
	VoxelizerContext saveContext = voxelizerGetCurrentContext();

	voxelizerMakeCurrent(voxelizerDeviceContext, voxelizerContext);  // use voxelizer's GL context

	voxelizer->meshVertCount = meshVertCount;
	voxelizer->meshVertPositions = meshVertPositions;

	glGenBuffers(1, &(voxelizer->meshVertBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, voxelizer->meshVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, meshVertCount * 3 * sizeof(float), meshVertPositions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	voxelizerMakeCurrent(saveDeviceContext, saveContext);  // restore caller's GL context
}

void voxelizerRun(Voxelizer *voxelizer)
{
	VoxelizerDeviceContext saveDeviceContext = voxelizerGetCurrentDeviceContext();  // save caller's GL context
	VoxelizerContext saveContext = voxelizerGetCurrentContext();

	voxelizerMakeCurrent(voxelizerDeviceContext, voxelizerContext);  // use voxelizer's GL context

	switch (voxelizer->algorithm)
	{
	case VOXELIZER_ABUFFER:
		voxelizerRunABuffer(voxelizer);
		break;
	}

	voxelizerMakeCurrent(saveDeviceContext, saveContext);  // restore caller's GL context
}