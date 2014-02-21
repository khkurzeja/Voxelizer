// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#version 420

in layout(location = 0) vec3 position;

uniform mat4 modelTransformMatrix;

void main() 
{
	vec4 v = vec4(position, 1.0f);
	gl_Position = modelTransformMatrix * v;
}