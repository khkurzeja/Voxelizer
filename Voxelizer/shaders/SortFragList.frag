// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#version 420

#extension GL_ARB_shader_storage_buffer_object : enable

uniform int framebufferWidth;

layout( std430, binding = 1 ) buffer HeadPointer
{
	int HeadPointers[];
};

layout( std430, binding = 2 ) buffer NodePointer
{
	int NodePointers[];
};

layout( std430, binding = 3 ) buffer NodeDatum
{
	int NodeData[];
};

void main() 
{
	int headPointerIndex = int(gl_FragCoord.x) + int(gl_FragCoord.y) * framebufferWidth;
	
	int headPtr = HeadPointers[headPointerIndex];
	if (headPtr == -1)
		discard;
	
	int outerPtr = headPtr;
	
	int prevInfLoop = 0;
	while ( prevInfLoop < 1024 && outerPtr != -1 )
	{
		int smallestPtr = outerPtr;
		int smallestData = NodeData[smallestPtr];
	
		int innerPtr = NodePointers[outerPtr];
		
		int prevInfLoopInner = 0;
		while ( prevInfLoopInner < 1024 && innerPtr != -1 )
		{
			int innerData = NodeData[innerPtr];
			if ( innerData < smallestData )
			{
				smallestPtr = innerPtr;
				smallestData = innerData;
			}
		
			innerPtr = NodePointers[innerPtr];
			
			prevInfLoopInner++;
		}
		
		if ( smallestPtr != outerPtr )
		{
			NodeData[smallestPtr] = NodeData[outerPtr];
			NodeData[outerPtr] = smallestData;
		}
		
		outerPtr = NodePointers[outerPtr];
		
		prevInfLoop++;
	}
	
	// create voxel 3d tex
	//memoryBarrier();
	
	
	
    gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}