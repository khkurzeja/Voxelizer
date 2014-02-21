// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#include <stdio.h>
#include "Voxelizer.h"
#include "Voxelizer_ABuffer.h"

#define VOXEL_GRID_SIZE 256

int main(int argc, char** argv)
{
	char strvar[100];
	int i;
	int j;

	Voxelizer_ABufferLinkedList *linkedList;

	// make tetrahdron
	float triSize = .5f;
	int meshVertCount = 12;
	float meshVertPositions[] = {
		-triSize, -triSize, -triSize,  // triangle 1
		 triSize, -triSize, -triSize,
		 triSize,  triSize, -triSize,

		-triSize, -triSize, -triSize,  // 2
		 triSize,  triSize, -triSize,
		 triSize, -triSize,  triSize,

		-triSize, -triSize, -triSize,  // 3
		 triSize, -triSize,  triSize,
		 triSize, -triSize, -triSize,

		 triSize,  triSize, -triSize,  // 4
		 triSize, -triSize, -triSize,
		 triSize, -triSize,  triSize
	};

	Voxelizer *voxelizer = voxelizerCreate(VOXELIZER_ABUFFER, VOXEL_GRID_SIZE, VOXEL_GRID_SIZE, VOXEL_GRID_SIZE);
	voxelizerSetMesh(voxelizer, meshVertCount, meshVertPositions);

	voxelizerRun(voxelizer);
	linkedList = voxelizerGetABufferLinkedList(voxelizer);

	// Voxelizer only returns a linked list representing the voxels at the moment.
	// Until other representations are supported the code below can be used to determine which voxels are in the mesh.
	for ( j = 0; j < voxelizer->height; j++ )
	{
		for ( i = 0; i < voxelizer->width; i++ )
		{
			int curPtr = linkedList->headPointers[i + j * voxelizer->width];

			while ( curPtr != -1 && linkedList->nodePointers[curPtr] != -1 )  // make sure there are at least 2 intersections ahead
			{
				int startVoxel = linkedList->nodeData[curPtr];                          // include
				int endVoxel = linkedList->nodeData[linkedList->nodePointers[curPtr]];  // exclude
				
				printf("Col: %d, Row: %d - Start voxel: %d, End Voxel: %d\n", i, j, startVoxel, endVoxel);  // every number between start and end voxel represents a voxel in column i, row j

				curPtr = linkedList->nodePointers[linkedList->nodePointers[curPtr]];
			}
		}
	}

	fgets(strvar, 100, stdin); // wait for input

	voxelizerDestroy(voxelizer);

	return 1;
}