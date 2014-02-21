// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#version 420

layout (depth_any) out float gl_FragDepth; 

layout(binding = 0) uniform atomic_uint counter;

void main() 
{
	atomicCounterIncrement(counter);
	
	gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}