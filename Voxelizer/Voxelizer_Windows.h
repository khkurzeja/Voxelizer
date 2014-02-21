// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#pragma once
#ifndef H_VOXELIZER_WINDOWS
#define H_VOXELIZER_WINDOWS

#include <Windows.h>

typedef HWND VoxelizerWindow;
typedef HDC VoxelizerDeviceContext;
typedef HGLRC VoxelizerContext;

VoxelizerWindow voxelizerCreateWindow();
int voxelizerDestroyWindow(VoxelizerWindow window);

void voxelizerInitContext(VoxelizerWindow window, VoxelizerDeviceContext *deviceContext, VoxelizerContext *context);
void voxelizerDestroyContext(VoxelizerWindow window, VoxelizerDeviceContext deviceContext, VoxelizerContext context);

VoxelizerDeviceContext voxelizerGetCurrentDeviceContext();
VoxelizerContext voxelizerGetCurrentContext();
int voxelizerMakeCurrent(VoxelizerDeviceContext deviceContext, VoxelizerContext context);

#endif