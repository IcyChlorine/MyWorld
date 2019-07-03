#pragma once
#ifndef _WIDGET_CLASSES_H

#include"stdafx.h"
#include"gl\MyGLToolkit.h"
#include"FrameworkClasses.h"

typedef string result;//�����㶮�ҵ���˼��8
#define nullstr string{""}
class GWidget//G����Game
{
protected:
	RECT area;//widget ���ڵ�λ�ü���ռ������
	GLfloat color_R{ 0.0f }, 
		color_G{ 0.8f },
		color_B{ 1.0f }, 
		color_A{ 1.0f };//Ĭ����ɫ

	bool explicit_proj_mode_setting{ false };//�Ƿ���ÿ����Ⱦʱǿ������ͶӰģʽ
	
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
	//��Ϊ���ı�������ֺ�ʱ�����ʹ��ͬһ��gltTextRenderer,�����е���ʾ�б�
	//��Ҫ�ؽ����ܷ�ʱ
	//��˼ƻ��Ѳ�ͬ������/�ֺ��ò�ͬ��gltTextRenderer��Ⱦ����ͬ������������mode������ʾ
	//�ƻ��������ɸ�mode
	string text;
	//string font{ "Comic Sans MS" };
	//pixel size{ 20 };
	unsigned int align_mode{ GLT_DEFAULT };//���뷽ʽ��default�����Ŵ����Ͻǿ�ʼ���룬
										   //	MID�������ĵ���롣
	unsigned int mode{ 0 };
public:
	GText();
	GText(string text, unsigned int mode = 0);
	virtual ~GText() {}

	//the constructive function of the static members
	static void GText_static();
	static void AddMode(string font, unsigned int size);
	
	//set��get����
	//void SetFont(string font_new);
	//void SetSize(unsigned int size_new);
	void SetText(string text_new);
	void SetMode(unsigned int mode_new);
	//������ʾ����ʱ���ֵ�λ�á�(x,y)���������Ͻ���Դ��ڿͻ�ȥ���Ͻǵ����꣬������Ϊ��λ
	//��ǰ���ǣ��������ڴ���GLT_2Dģʽ�¡�
	void SetAlignMode(unsigned int mode_new);
	//inline int GetSize() { return size; }

	virtual void Render();
};

//�����ѵĲ�������
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

	//set/get����
	void SetCaption(string caption_new);
	void SetFunction(string type, pfunc func);
	
	//�ж�һ�����ǲ�����button��Χ��
	bool WinthinBoundary(pixel x, pixel y);

	//�ص�����/�¼�����
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

	//��ӿؼ��ĺ�����ע�⣺��ӽ�����ֻ�ǿؼ���ָ�룬���ǿ���
	void AttachWidget(GWidget* w);

	//��Ϊ�ؼ���Ⱦ
	virtual void Render();

	//��Ϊ��������
	string SingleLoop();//ֻѭ��һ�ε�Run
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
		glBegin(GL_LINES);							// ���Ƹ�������ʮ����
		glVertex2f(0, (float)pWnd->GetConsole().mouse.y);
		glVertex2f((float)pWnd->GetWidth(), (float)pWnd->GetConsole().mouse.y);
		glVertex2f((float)pWnd->GetConsole().mouse.x, 0);
		glVertex2f((float)pWnd->GetConsole().mouse.x, (float)pWnd->GetHeight());
		glEnd();								// ���ƽ���

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
