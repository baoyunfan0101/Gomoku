#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include "framework.h"
#include "AI-arith.h"

/* 记录当前状态信息的变量指针 */
state* t;

/* 定义表示棋子颜色的宏 */
#define UNKNOW 0
#define BLACK -1
#define WHITE 1

/* 定义窗口宽度和高度 */
#define MY_WIDTH 700
#define MY_HEIGHT 670

/* 定义一个棋子 */
LONG pieceWidth = 100;	//棋子宽度
typedef struct PIECE {
	int i;
	int j;
	int color;
}Piece[255];
Piece pieces;
int ip = 0;	//当前棋子数

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	int i, j;

	switch (msg)
	{
		/* 窗口大小改变 */
	case WM_SIZE:
		static int cxClient = LOWORD(lParam);
		static int cyClient = HIWORD(lParam);
		//获取棋子宽度
		pieceWidth = cxClient / 18;
		return 0;

		/* 单击左键 */
	case WM_LBUTTONDOWN:
		POINT ptClick;

		//获取单击位置
		ptClick.x = GET_X_LPARAM(lParam);
		ptClick.y = GET_Y_LPARAM(lParam);

		//计算棋子安放位置
		i = (ptClick.x + pieceWidth / 2) / pieceWidth - 2;
		j = (ptClick.y + pieceWidth / 2) / pieceWidth - 1;

		//判断棋子安放位置是否合理
		if (i < 0 || i > 14 || j < 0 || j > 14 || t->CB[i][j] != UNKNOW)
			return 0;

		//将对手行棋载入棋子队列
		pieces[ip].i = i;
		pieces[ip].j = j;
		pieces[ip].color = BLACK;
		t->CB[i][j] = BLACK;
		ip++;

		//重绘
		SetRect(&rect, pieceWidth * (i + 2) - pieceWidth / 2, pieceWidth * (j + 1) - pieceWidth / 2,
			pieceWidth * (i + 2) + pieceWidth / 2, pieceWidth * (j + 1) + pieceWidth / 2);
		InvalidateRect(hWnd, &rect, FALSE);

		//目标测试
		if (t->GoalTest() == BLACK)
			if (MessageBox(hWnd, TEXT("你真棒！"), TEXT("五子棋小游戏"), MB_OK) == IDOK)
				DestroyWindow(hWnd);

		//搜索应对策略
		t = t->minimax(2);	//搜索应对策略
		t->clear();	//重新初始化其上一次搜索的临时数据

		//将AI行棋载入棋子队列
		pieces[ip].i = t->Last_i;
		pieces[ip].j = t->Last_j;
		pieces[ip].color = WHITE;
		t->CB[t->Last_i][t->Last_j] = WHITE;
		ip++;

		//重绘
		SetRect(&rect, pieceWidth * (t->Last_i + 2) - pieceWidth / 2, pieceWidth * (t->Last_j + 1) - pieceWidth / 2,
			pieceWidth * (t->Last_i + 2) + pieceWidth / 2, pieceWidth * (t->Last_j + 1) + pieceWidth / 2);
		InvalidateRect(hWnd, &rect, FALSE);

		//目标测试
		if (t->GoalTest() == WHITE)
			if (MessageBox(hWnd, TEXT("你输了，别气馁！"), TEXT("五子棋小游戏"), MB_OK) == IDOK)
				DestroyWindow(hWnd);

		return 0;

		/* 单击右键 */
	case WM_RBUTTONDOWN:
		if (ip == 0)
			return 0;

		for (int k = 0; k < 2; ++k) {
			//重置对手行棋
			ip--;
			i = pieces[ip].i;
			j = pieces[ip].j;
			t->CB[i][j] = UNKNOW;
			pieces[ip].i = 0;
			pieces[ip].j = 0;
			pieces[ip].color = UNKNOW;

			//重绘
			SetRect(&rect, pieceWidth * (i + 2) - pieceWidth / 2, pieceWidth * (j + 1) - pieceWidth / 2,
				pieceWidth * (i + 2) + pieceWidth / 2, pieceWidth * (j + 1) + pieceWidth / 2);
			InvalidateRect(hWnd, &rect, TRUE);
		}

		return 0;

		/* 窗口等待刷新 */
	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		//绘制棋盘
		for (int i = 0; i < 15; ++i) {
			MoveToEx(hdc, pieceWidth * (i + 2), pieceWidth * 1, NULL);
			LineTo(hdc, pieceWidth * (i + 2), pieceWidth * 15);
		}
		for (int j = 0; j < 15; ++j) {
			MoveToEx(hdc, pieceWidth * 2, pieceWidth * (j + 1), NULL);
			LineTo(hdc, pieceWidth * 16, pieceWidth * (j + 1));
		}

		//绘制棋子
		HBRUSH hBlackBrush, hWhiteBrush;
		hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
		hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
		for (int i = 0, j = 0, k = 0; k < ip; ++k) {
			if (pieces[k].color == UNKNOW)
				continue;
			if (pieces[k].color == BLACK)
				SelectObject(hdc, hBlackBrush);
			if (pieces[k].color == WHITE)
				SelectObject(hdc, hWhiteBrush);
			int m = pieces[k].i, n = pieces[k].j;
			Ellipse(hdc, pieceWidth * (m + 2) - pieceWidth / 2, pieceWidth * (n + 1) - pieceWidth / 2,
				pieceWidth * (m + 2) + pieceWidth / 2, pieceWidth * (n + 1) + pieceWidth / 2);
		}
		EndPaint(hWnd, &ps);

		return 0;

		/* 窗口关闭 */
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("游戏结束，感谢支持！"), TEXT("五子棋小游戏"), MB_OK) == IDOK)
			DestroyWindow(hWnd);

		return 0;

		/* 窗口被摧毁 */
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;

		/* 单击按钮/菜单项/工具栏 */
	case WM_COMMAND:
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			MessageBox(hWnd, TEXT("游戏操作：\n\t单击左键 - 落子\n\t单击右键 - 悔棋\n\n游戏制作：\n\t包云帆"),
				TEXT("五子棋小游戏"), MB_OK);
			break;
		case IDM_EXIT:
			if (MessageBox(hWnd, TEXT("游戏结束，感谢支持！"), TEXT("五子棋小游戏"), MB_OK) == IDOK)
				DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR szCmdLine, int iCmdShow)
{
	/* 声明算法内部变量 */
	t = new state;

	/* 定义窗口类 */
	WNDCLASS wndClass;	//定义窗口类
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//窗口类风格：调整窗口大小时是否重绘
	wndClass.lpfnWndProc = WndProc;	//窗口过程函数
	wndClass.cbClsExtra = 0;	//指定窗口类结构后要分配的额外字节数
	wndClass.cbWndExtra = 0;	//指定窗口实例结构后要分配的额外字节数
	wndClass.hInstance = hInstance;	//实例句柄
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	//光标
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//背景刷
	wndClass.lpszMenuName = MAKEINTRESOURCEW(IDC_FIVEINAROW);	//菜单
	wndClass.lpszClassName = TEXT("人工智能原理与技术");	//窗口类名称

	/* 注册窗口类 */
	if (!RegisterClass(&wndClass)) {
		MessageBox(NULL, TEXT("窗口注册失败！"), TEXT("人工智能原理与技术"), MB_ICONERROR);
		return 0;
	}

	/* 创建窗口 */
	HWND hWnd = CreateWindow(TEXT("人工智能原理与技术"), TEXT("五子棋小游戏"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		0, 0, MY_WIDTH, MY_HEIGHT,
		NULL, NULL, hInstance, NULL);

	/* 显示窗口 */
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	/* 消息循环 */
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);	//调用窗口过程函数
	}

	return msg.wParam;
}