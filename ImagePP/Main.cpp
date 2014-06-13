

#include "stdio.h"
#include "DecoderTga.h"
#include "HCStdStream.h"
#include "ImagePP.h"
#include <windows.h>
PIXELFORMATDESCRIPTOR pfd =
{
	sizeof(PIXELFORMATDESCRIPTOR), // size
	1,       // version
	PFD_SUPPORT_OPENGL |  // OpenGL window
	PFD_DRAW_TO_WINDOW |  // render to window
	PFD_DOUBLEBUFFER,   // support double-buffering
	PFD_TYPE_RGBA,    // color type
	32,       // prefered color depth
	0, 0, 0, 0, 0, 0,   // color bits (ignored)
	0,       // no alpha buffer
	0,       // alpha bits (ignored)
	0,       // no accumulation buffer
	0, 0, 0, 0,     // accum bits (ignored)
	16,       // depth buffer
	0,       // no stencil buffer
	0,       // no auxiliary buffers
	PFD_MAIN_PLANE,    // main layer
	0,       // reserved
	0, 0, 0,     // no layer, visible, damage masks
};
Image* im = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HDC hMemDC;
	int pixelFormat;
	HBITMAP bitmap;
	ImagePP* ipp;// 
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd);
		pixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pixelFormat, &pfd);
		ipp = new ImagePP();
		im=ipp->CreateFromFile("1.tga");
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (im != NULL)
		{
			
			hMemDC = ::CreateCompatibleDC(hdc);
			bitmap = ::CreateBitmap(im->GetWidth(), im->GetHeight(),1,32,im->GetData());
			::SelectObject(hMemDC, bitmap);
			::BitBlt(hdc, 0, 0, im->GetWidth(), im->GetHeight(), hMemDC, 0, 0, SRCCOPY);
			::DeleteDC(hMemDC);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
int APIENTRY WinMain(HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR    lpCmdLine,
	 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_IMAGEWND, szWindowClass, MAX_LOADSTRING);
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGEWND));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCE(IDC_IMAGEWND);
	wcex.lpszClassName = L"CLASSTEST";
	wcex.hIconSm = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	//hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindow(L"CLASSTEST", L"CCC", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);



	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	//	}
	}

	return (int)msg.wParam;
}