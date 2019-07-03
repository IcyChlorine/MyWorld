/*
	FrameworkClasses.h
	�����.h
	������
		���ṹ�����ƽṹ�嶨��
		GL������
*/
#pragma once
#ifndef _FRAMEWORK_CLASSES_H
#include"stdafx.h"

//�����Ϣ�ṹ
struct Mouse
{
	int x{ 0 }, y{ 0 };
	bool LButton{ false }, RButton{ false };
	bool EventLButtonDown{ false }, EventRButtonDown{ false };//�ĸ������Ե��¼�
	bool EventLButtonUp{ false }, EventRButtonUp{ false };
};
//�û�������Ϣ�ṹ�壬�����+������Ϣ
struct Console
{
	bool keys[256];
	Mouse mouse;
	Console()
	{
		memset((void*)keys, false, sizeof(bool)*256);
	}
};

//���ڹ��̺�������
LRESULT CALLBACK
_WndProc(HWND hWnd,				
	UINT	uMsg,							
	WPARAM wParam,				
	LPARAM	 lParam);

//�ҵ� ������
class GLWindow
{
public:
	//�����Ϣ
	HINSTANCE hInstance{ nullptr };
	HWND hWnd{ nullptr };
	HDC hDC{ nullptr };
	HGLRC hRC{ nullptr };

	//���ڻ�����Ϣ
	//������λ�����أ�
	int width{ 0 }, height{ 0 };
	//�Ƿ񼤻�
	bool active{ true };
	//�Ƿ���ʾ���
	bool show_cursor{ true };
	//�˳���־
	bool quit{ false };

	//������Ϣ
	Console cmd;

public:
	GLWindow(string caption, int width, int height);
	GLWindow(const GLWindow&) = delete;
	~GLWindow();
	void Update();//����Ϣѭ��������
	bool Quit() { return quit; }
	
private:
	friend LRESULT CALLBACK _WndProc(HWND hWnd, UINT	uMsg, WPARAM wParam, LPARAM	 lParam);
	LRESULT CALLBACK _MyWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//��ʼ����غ���
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
extern GLWindow* pWnd;//ȫ��Ψһ�Ĵ��ڵ�ָ�룬ʵ�ʶ�����myworld.cpp

struct GC//LIKE GLRC,DC, GC means "game context".
{
	Console* cmd;
	GLWindow* wnd;
};
typedef  GC* HGC;
#endif // !_FRAMEWORK_CLASSES_H
