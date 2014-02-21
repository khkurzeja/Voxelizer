// Copyright (c) 2014 Kelsey Kurzeja (khkurzeja)
// This code is licensed under the MIT license (see LICENSE.txt for details)

#include "Voxelizer_Windows.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return 0;
		
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		
	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);		
	}	
}

VoxelizerWindow voxelizerCreateWindow()
{
	HINSTANCE hInstance;
	WNDCLASS wc;
	HWND hWnd;

	hInstance = GetModuleHandle(0);

	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "VoxelizerWindow";
	RegisterClass(&wc);

	hWnd = CreateWindow( 
		"VoxelizerWindow", "Voxelizer", 
		0,
		0, 0, 100, 100,
		NULL, NULL, hInstance, NULL 
	);

	return hWnd;
}

int voxelizerDestroyWindow(VoxelizerWindow window)
{
	int success = DestroyWindow(window);
	if (success)
		return 1;
	else
		return 0;
}

void voxelizerInitContext(VoxelizerWindow window, VoxelizerDeviceContext *deviceContext, VoxelizerContext *context)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	
	*deviceContext = GetDC(window);
	
	// set the pixel format for the device context
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(*deviceContext, &pfd);
	SetPixelFormat(*deviceContext, format, &pfd);
	
	*context = wglCreateContext(*deviceContext);  // might want to use wglCreateContextAttribsARB in the future
}

void voxelizerDestroyContext(VoxelizerWindow window, VoxelizerDeviceContext deviceContext, VoxelizerContext context)
{
	wglDeleteContext(context);
	ReleaseDC(window, deviceContext);
}

VoxelizerDeviceContext voxelizerGetCurrentDeviceContext()
{
	return wglGetCurrentDC();
}

VoxelizerContext voxelizerGetCurrentContext()
{
	return wglGetCurrentContext();
}

int voxelizerMakeCurrent(VoxelizerDeviceContext deviceContext, VoxelizerContext context)
{
	int success = wglMakeCurrent(deviceContext, context);
	if (success)
		return 1;
	else
		return 0;
}