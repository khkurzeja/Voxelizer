// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#version 420

#extension GL_ARB_shader_storage_buffer_object : enable

layout (depth_any) out float gl_FragDepth; 

uniform int framebufferWidth;

layout(binding = 0) uniform atomic_uint counter;

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
	uint ucount = atomicCounterIncrement(counter);
	int count = int(ucount);
	
	int headPointerIndex = int(gl_FragCoord.x) + int(gl_FragCoord.y) * framebufferWidth;
	float depth = gl_FragCoord.z;
	
	int prevPtr = atomicExchange(HeadPointers[headPointerIndex], count);
	NodePointers[count] = prevPtr;
	NodeData[count] = int(depth * framebufferWidth);
	
	gl_FragColor = vec4(1.0f);
}