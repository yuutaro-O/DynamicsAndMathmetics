#ifndef INCLUDED_DXLENDERLING_H_
#define INCLUDED_DXLENDERLING_H_
	#include <cstdio>
	#include <Windows.h>
	#include <tchar.h>

	#include <D3D11.h>
	#include <D3DX11.h>
	#include <D3Dcompiler.h>
	#include <xnamath.h>

	#include "CustomStruct.h"

	HRESULT InitD3D(void);
	HRESULT MakeShaders(void);
	int InitDrawModes(void);
	HRESULT InitGeometry(void);
	int Cleanup(void);
	LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int FlushDrawingPictures(void);
	int DrawPoints(int x, int y, RGBAColor color);
	int DrawPoints(int x, int y, int nRed, int nGreen, int nBlue);
	HRESULT Render(void);
	int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, int);
#endif // !INCLUDED_DXLENDERLING_H_

