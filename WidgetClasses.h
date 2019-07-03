#pragma once
#ifndef _WIDGET_CLASSES_H

#include"stdafx.h"
#include"gl\MyGLToolkit.h"
#include"FrameworkClasses.h"

typedef string result;//这下你懂我的意思了8
#define nullstr string{""}
class GWidget//G代表Game
{
protected:
	RECT area;//widget 所在的位置及所占的区域
	GLfloat color_R{ 0.0f }, 
		color_G{ 0.8f },
		color_B{ 1.0f }, 
		color_A{ 1.0f };//默认颜色

	bool explicit_proj_mode_setting{ false };//是否在每次渲染时强制重置投影模式
	
public:
	GWidget();
	virtual ~GWidget() {}
	
	virtual string _GetKind() { return ""; }

	void ExplicitProjectionModeSetting(bool b);
	void SetPos(pixel x_new, pixel y_new);
	void SetColor(float R_new, float G_new, float B_new, float A_new = -1.0f);
	void SetTransparancy(float transparancy);
	void SetSize(pixel width, pixel height);
	virtual void Render();
};
class GRectangle :public GWidget
{
private:
public:
	GRectangle(pixel left, pixel top, pixel right, pixel bottom);
	GRectangle() {}
	virtual ~GRectangle() {}

	virtual void Render();
};
class GBackground:public GWidget
{
private:
	HTEX background;
public:
	GBackground(string filepath);
	void SetSize(pixel width, pixel height);
	virtual void Render();
};
class GText :public GWidget
{
private:
	static vector<gltTextRenderer> renderers;
	//因为当改变字体和字号时，如果使用同一个gltTextRenderer,那所有的显示列表
	//都要重建，很费时
	//因此计划把不同的字体/字号用不同的gltTextRenderer渲染，不同的字体设置用mode参量表示
	//计划内置若干个mode
	string text;
	//string font{ "Comic Sans MS" };
	//pixel size{ 20 };
	unsigned int align_mode{ GLT_DEFAULT };//对齐方式。default代表着从左上角开始对齐，
										   //	MID代表按中心点对齐。
	unsigned int mode{ 0 };
public:
	GText();
	GText(string text, unsigned int mode = 0);
	virtual ~GText() {}

	//the constructive function of the static members
	static void GText_static();
	static void AddMode(string font, unsigned int size);
	
	//set和get函数
	//void SetFont(string font_new);
	//void SetSize(unsigned int size_new);
	void SetText(string text_new);
	void SetMode(unsigned int mode_new);
	//设置显示文字时文字的位置。(x,y)是文字左上角相对窗口客户去左上角的坐标，以像素为单位
	//但前提是，整个窗口处于GLT_2D模式下。
	void SetAlignMode(unsigned int mode_new);
	//inline int GetSize() { return size; }

	virtual void Render();
};

//最困难的部分来了
typedef string(*pfunc)(void*);
class GButton :public GWidget
{
private:
	string caption{ "button" };
	bool is_cursor_passing{ false };
	bool is_cursor_clicking{ false };
	pfunc func_cursor_passing{ nullptr };
	pfunc func_cursor_clicking{ nullptr };
	pfunc func_mouse_down{ nullptr };
	pfunc func_mouse_up{ nullptr };
	
public:
	GButton() {}
	GButton(string caption) :caption{ caption } {}
	GButton(pfunc cb_func, string caption) :func_mouse_up{ cb_func }, caption{ caption } {}
	virtual ~GButton() {}

	virtual string _GetKind();

	//set/get函数
	void SetCaption(string caption_new);
	void SetFunction(string type, pfunc func);
	
	//判断一个点是不是在button范围内
	bool WinthinBoundary(pixel x, pixel y);

	//回调函数/事件函数
	string OnCursorPassing(void* param);
	string OnCursorClicking(void* param);
	string OnMouseDown(void* param);
	string OnMouseUp(void* param);

	virtual void Render();
};
class GContainer:public GWidget
{
protected:
	vector<GWidget*> widgets;
	vector<GButton*> buttons;
public:
	//Constructor & Deconstructor
	GContainer(){}
	virtual ~GContainer() {}

	//添加控件的函数。注意：添加进来的只是控件的指针，不是拷贝
	void AttachWidget(GWidget* w);

	//作为控件渲染
	virtual void Render();

	//作为窗体运行
	string SingleLoop();//只循环一次的Run
	string Run();
};

/*class GUI :public GWidget
{
private:
public:
	GUI(){}
	virtual void Render()
	{
		if (pWnd->GetConsole().mouse.LButton)
			glColor3f(1.0f, 0.1f, 0.2f);
		else
			glColor3f(0.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);							// 绘制跟随鼠标的十字线
		glVertex2f(0, (float)pWnd->GetConsole().mouse.y);
		glVertex2f((float)pWnd->GetWidth(), (float)pWnd->GetConsole().mouse.y);
		glVertex2f((float)pWnd->GetConsole().mouse.x, 0);
		glVertex2f((float)pWnd->GetConsole().mouse.x, (float)pWnd->GetHeight());
		glEnd();								// 绘制结束

		gltRenderText(string{ "testttt" },
			pWnd->GetConsole().mouse.x,
			pWnd->GetConsole().mouse.y);
	}
};*/

class GWelcomePage :public GContainer
{
private:
	static string func_game_start(void* param)
	{
		return string{ "game_start" };
	}
	static string func_quit(void* param)
	{
		return string{ "quit" };
	}
public:

	GWelcomePage()
	{
		GBackground* bg=new GBackground{ "src\\background.jpg" };
		bg->SetSize(1440, 900);

		GButton* b=new GButton{ string{ "Game Start" } };
		b->SetSize(200, 50);
		b->SetPos(720 - 100, 300);
		b->SetFunction(string("mouse_up"), func_game_start);

		GButton* b2=new GButton{ *b };
		b2->SetCaption(string{ "Quit" });
		b2->SetPos(720 - 100, 500);
		b2->SetFunction(string{ "mouse_up" }, func_quit);

		GText* t=new GText{ "MyWorld",5 };//title
		t->SetAlignMode(GLT_DEFAULT);
		t->SetPos(620, 80);
		t->SetColor(0.9f, 0.9f, 0.95f);
		GText* t_s=new GText{ *t };//shadow of title
		t_s->SetColor(0.0f, 0.0f, 0.0f);
		t_s->SetPos(625, 85);

		//GUI* gui=new GUI;

		(this)->AttachWidget(bg);
		AttachWidget(b);
		AttachWidget(b2);
		AttachWidget(t_s);
		AttachWidget(t);
		//AttachWidget(gui);
	}
	virtual ~GWelcomePage()
	{
		for (GWidget* p : widgets)
			delete p;
	}
};






#endif // !_WIDGET_CLASSES_H
