#pragma once
#ifndef _GLT_H
#define _GLT_H

#include"..\stdafx.h"



//声明

#define GLT_2D 0x0001
#define GLT_3D 0x0002
#define GLT_DEFAULT 0x0000
#define GLT_MID 0x0003

typedef GLuint HTEX;
typedef GLuint HLIST;

typedef unsigned int pixel;//定义类型：像素单位
void gltSetProjectionMode(unsigned int mode, int wnd_width=-1, int wnd_height=-1);
unsigned int& gltGetProjectionMode();
//文字渲染相关函数
/*
PS:只能渲染ascii字符，不能渲染中文字；可以设置文字的字体、位置、颜色、字号/大小（单位：像素）
在重新设置字体和字号的时候，会重新初始化字体，比较慢。
最底层用了一个gltTextRenderer对象。但通过gltGetTextRenderer函数+静态局部变量把全局对象藏起来了，
并通过一系列函数包装成了opengl库函数的风格。
*/
class gltTextRenderer
{
private:
	float color[3];//RGB, [0.0f,1.0f]^3
	string font{ "Comic Sans MS" };
	const pixel std_size{ 20 };
	pixel size{ 0 };
	pixel pos[2];//文字位置，相对左上角的坐标，单位为像素
	unsigned int align_mode{ GLT_DEFAULT };//对齐方式。default代表着从左上角开始对齐，
										   //	MID代表按中心点对齐。

	TEXTMETRIC tm;//字符的一些具体参数，备用

	GLuint _char_list_entry{ 0 };//存放渲染字体的显示列表的变量
	HFONT hFont{ nullptr };

	static gltTextRenderer* pPrevObj;//a var flagging whether mode is shifted
	
public:
	void Update();//重新初始化
	
	gltTextRenderer();
	gltTextRenderer(string font, unsigned int size);
	~gltTextRenderer();

	//核心函数！！！
	void RenderText(string text);

	//set和get函数
	void SetFont(string font_new);
	void SetColor(float R, float G, float B);
	void SetSize(pixel size_new);
	void SetPos(pixel x, pixel y);
	//设置显示文字时文字的位置。(x,y)是文字左上角相对窗口客户去左上角的坐标，以像素为单位
	//但前提是，整个窗口处于GLT_2D模式下。
	void SetAlignMode(unsigned int mode_new);
	inline	int GetSize();
};
gltTextRenderer& gltGetTextRenderer();
void gltTextColor(float R, float G, float B);
void gltTextPos(pixel x, pixel y);
void gltTextFont(string font_new);
void gltTextSize(pixel size_new);
void gltTextAlignMode(unsigned int mode_new);
void gltRenderText(string text);
void gltRenderText(string text, pixel x, pixel y);

class gltTextureHelper;
gltTextureHelper& gltGetTextureHelper();
HTEX gltLoadTexture(string filepath,unsigned int* pw=nullptr,unsigned int* ph=nullptr);

void gltBeginRender();
void gltEndRender();

void gltShowLoadingPage();


#endif // !_GLT_H
