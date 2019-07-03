/*
	FrameworkClasses.h
	框架类.h
	包含：
		鼠标结构、控制结构体定义
		GL窗口类
*/
#pragma once
#ifndef _FRAMEWORK_CLASSES_H
#include"stdafx.h"

//鼠标消息结构
struct Mouse
{
	int x{ 0 }, y{ 0 };
	bool LButton{ false }, RButton{ false };
	bool EventLButtonDown{ false }, EventRButtonDown{ false };//四个单次性的事件
	bool EventLButtonUp{ false }, EventRButtonUp{ false };
};
//用户控制信息结构体，即鼠标+键盘信息
struct Console
{
	bool keys[256];
	Mouse mouse;
	Console()
	{
		memset((void*)keys, false, sizeof(bool)*256);
	}
};

//窗口过程函数声明
LRESULT CALLBACK
_WndProc(HWND hWnd,				
	UINT	uMsg,							
	WPARAM wParam,				
	LPARAM	 lParam);

//我的 窗口类
class GLWindow
{
public:
	//句柄信息
	HINSTANCE hInstance{ nullptr };
	HWND hWnd{ nullptr };
	HDC hDC{ nullptr };
	HGLRC hRC{ nullptr };

	//窗口基本信息
	//长宽（单位：像素）
	int width{ 0 }, height{ 0 };
	//是否激活
	bool active{ true };
	//是否显示鼠标
	bool show_cursor{ true };
	//退出标志
	bool quit{ false };

	//控制信息
	Console cmd;

public:
	GLWindow(string caption, int width, int height);
	GLWindow(const GLWindow&) = delete;
	~GLWindow();
	void Update();//起到消息循环的作用
	bool Quit() { return quit; }
	
private:
	friend LRESULT CALLBACK _WndProc(HWND hWnd, UINT	uMsg, WPARAM wParam, LPARAM	 lParam);
	LRESULT CALLBACK _MyWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//初始化相关函数
	void _MyRegisterWindowClass();
	void _MyCreateWindow(string caption,int width,int height);
	void _MyCreateGLWindow(string caption,int width,int height);
	void _InitGLContext();
	void _OnResize(int width, int height);

	void static _JudgeUnique();

public:
	HWND GetHWnd() const { return hWnd; }
	HDC GetHDC() const { return hDC; }
	HGLRC GetHRC() const { return hRC; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	bool IsActive() const { return active; }
	Console& GetConsole() { return cmd; }

	void SetCursorPos(int x, int y);
	void HideCursor();
	void ShowCursor(bool show=true);
};
typedef GLWindow* HGLWND;
extern GLWindow* pWnd;//全局唯一的窗口的指针，实际定义在myworld.cpp

struct GC//LIKE GLRC,DC, GC means "game context".
{
	Console* cmd;
	GLWindow* wnd;
};
typedef  GC* HGC;
#endif // !_FRAMEWORK_CLASSES_H
