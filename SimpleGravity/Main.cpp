/*
A simple gravity simulation.(Gravity, Elasticity)

========
Copyright 2016 Skyler Caron

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//Latest Edit: 10/27/16 11:54
#pragma once
#include <windows.h>
#include <time.h>
#include "BallLauncher.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("SimpleGravity");
LPCWSTR lpszTitle = TEXT("Simple Gravity");

HDC hdc, memHdc, backMemDC;
PAINTSTRUCT ps;

HBITMAP canvasBitmap, oldCanvasBitmap;

BallLauncher *launcher = NULL;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	g_hinst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 540, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) 를 GetMessage대신에 사용하면 메세지가 더이상 없을때 메세지 받기를 종료한다.
	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HBITMAP oldBitmap;
	static RECT winRect;//현재 윈도우 크기를 저장하는 변수입니다.

	switch (iMessage)
	{

	case WM_CREATE:
	{
		GetClientRect(hWnd, &winRect);
		srand((unsigned)time(0));

		launcher = new BallLauncher();
		launcher->groundHeight = 128;

		SetTimer(hWnd, 1, 2, NULL);
		return 0;
	}

	case WM_TIMER:
	{
		switch (wParam)
		{
		case 1:
		{
			launcher->Update();//Update all Balls
			break;
		}
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		launcher->Mouse(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		//Mouse Left Down: Create Ball
		//LOWORD(lParam);//마우스의 x좌표, HIWORD(lParam);//마우스의 y좌표.
		launcher->Mouse(LOWORD(lParam), HIWORD(lParam));
		launcher->Create();
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		//Mouse Right Down: Gravity On/Off
		launcher->Mouse(LOWORD(lParam), HIWORD(lParam));
		launcher->gravity = !launcher->gravity;
		return 0;
	}

	case WM_PAINT:
	{
		//DC: 화면에 무언가를 그릴 도구의 모음집
		hdc = BeginPaint(hWnd, &ps);
		memHdc = CreateCompatibleDC(hdc);//비트맵/브러시 등을 위한 HDC입니다.
		backMemDC = CreateCompatibleDC(hdc);//화면을 그려줄 HDC입니다.

											//더블버퍼의 비트맵들을 초기화시킵니다.
		canvasBitmap = CreateCompatibleBitmap(hdc, winRect.right, winRect.bottom);
		oldCanvasBitmap = (HBITMAP)SelectObject(backMemDC, canvasBitmap);


		//====BitBlt으로 그리는 부분입니다.====

		launcher->Draw(backMemDC, memHdc, oldBitmap, winRect);

		//========


		//SelectObject(memHdc, oldBitmap);

		BitBlt(hdc, 0, 0, winRect.right, winRect.bottom, backMemDC, 0, 0, SRCCOPY); //메모리(backMemDC)에 그렸던 화면을 메인 hdc로 복사시킵니다.

																					// 메모리의 화면 비트맵을 원래대로 한 후 제거합니다.
		DeleteObject(SelectObject(backMemDC, oldCanvasBitmap));

		//HDC들도 DeleteDC로 해제합니다.
		DeleteDC(memHdc);
		DeleteDC(backMemDC);
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_SIZE:
	{
		GetClientRect(hWnd, &winRect);//바뀐 윈도우 크기를 가져옵니다.
		launcher->Size(winRect.right, winRect.bottom);
		return 0;
	}

	case WM_DESTROY:
	{
		//플레이어를 해제시킵니다.
		delete launcher;

		//타이머를 해제시킵니다.
		KillTimer(hWnd, 1);

		//비트맵도 해제합니다.
		PostQuitMessage(0);
		return 0;
	}

	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));//위에서 return을 만나지 못했을 경우를 위한 예외 처리입니다.
}