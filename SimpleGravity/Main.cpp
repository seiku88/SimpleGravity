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

	//PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) �� GetMessage��ſ� ����ϸ� �޼����� ���̻� ������ �޼��� �ޱ⸦ �����Ѵ�.
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
	static RECT winRect;//���� ������ ũ�⸦ �����ϴ� �����Դϴ�.

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
		//LOWORD(lParam);//���콺�� x��ǥ, HIWORD(lParam);//���콺�� y��ǥ.
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
		//DC: ȭ�鿡 ���𰡸� �׸� ������ ������
		hdc = BeginPaint(hWnd, &ps);
		memHdc = CreateCompatibleDC(hdc);//��Ʈ��/�귯�� ���� ���� HDC�Դϴ�.
		backMemDC = CreateCompatibleDC(hdc);//ȭ���� �׷��� HDC�Դϴ�.

											//��������� ��Ʈ�ʵ��� �ʱ�ȭ��ŵ�ϴ�.
		canvasBitmap = CreateCompatibleBitmap(hdc, winRect.right, winRect.bottom);
		oldCanvasBitmap = (HBITMAP)SelectObject(backMemDC, canvasBitmap);


		//====BitBlt���� �׸��� �κ��Դϴ�.====

		launcher->Draw(backMemDC, memHdc, oldBitmap, winRect);

		//========


		//SelectObject(memHdc, oldBitmap);

		BitBlt(hdc, 0, 0, winRect.right, winRect.bottom, backMemDC, 0, 0, SRCCOPY); //�޸�(backMemDC)�� �׷ȴ� ȭ���� ���� hdc�� �����ŵ�ϴ�.

																					// �޸��� ȭ�� ��Ʈ���� ������� �� �� �����մϴ�.
		DeleteObject(SelectObject(backMemDC, oldCanvasBitmap));

		//HDC�鵵 DeleteDC�� �����մϴ�.
		DeleteDC(memHdc);
		DeleteDC(backMemDC);
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_SIZE:
	{
		GetClientRect(hWnd, &winRect);//�ٲ� ������ ũ�⸦ �����ɴϴ�.
		launcher->Size(winRect.right, winRect.bottom);
		return 0;
	}

	case WM_DESTROY:
	{
		//�÷��̾ ������ŵ�ϴ�.
		delete launcher;

		//Ÿ�̸Ӹ� ������ŵ�ϴ�.
		KillTimer(hWnd, 1);

		//��Ʈ�ʵ� �����մϴ�.
		PostQuitMessage(0);
		return 0;
	}

	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));//������ return�� ������ ������ ��츦 ���� ���� ó���Դϴ�.
}