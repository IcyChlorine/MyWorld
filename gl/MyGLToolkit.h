#pragma once
#ifndef _GLT_H
#define _GLT_H

#include"..\stdafx.h"



//����

#define GLT_2D 0x0001
#define GLT_3D 0x0002
#define GLT_DEFAULT 0x0000
#define GLT_MID 0x0003

typedef GLuint HTEX;
typedef GLuint HLIST;

typedef unsigned int pixel;//�������ͣ����ص�λ
void gltSetProjectionMode(unsigned int mode, int wnd_width=-1, int wnd_height=-1);
unsigned int& gltGetProjectionMode();
//������Ⱦ��غ���
/*
PS:ֻ����Ⱦascii�ַ���������Ⱦ�����֣������������ֵ����塢λ�á���ɫ���ֺ�/��С����λ�����أ�
����������������ֺŵ�ʱ�򣬻����³�ʼ�����壬�Ƚ�����
��ײ�����һ��gltTextRenderer���󡣵�ͨ��gltGetTextRenderer����+��̬�ֲ�������ȫ�ֶ���������ˣ�
��ͨ��һϵ�к�����װ����opengl�⺯���ķ��
*/
class gltTextRenderer
{
private:
	float color[3];//RGB, [0.0f,1.0f]^3
	string font{ "Comic Sans MS" };
	const pixel std_size{ 20 };
	pixel size{ 0 };
	pixel pos[2];//����λ�ã�������Ͻǵ����꣬��λΪ����
	unsigned int align_mode{ GLT_DEFAULT };//���뷽ʽ��default�����Ŵ����Ͻǿ�ʼ���룬
										   //	MID�������ĵ���롣

	TEXTMETRIC tm;//�ַ���һЩ�������������

	GLuint _char_list_entry{ 0 };//�����Ⱦ�������ʾ�б�ı���
	HFONT hFont{ nullptr };

	static gltTextRenderer* pPrevObj;//a var flagging whether mode is shifted
	
public:
	void Update();//���³�ʼ��
	
	gltTextRenderer();
	gltTextRenderer(string font, unsigned int size);
	~gltTextRenderer();

	//���ĺ���������
	void RenderText(string text);

	//set��get����
	void SetFont(string font_new);
	void SetColor(float R, float G, float B);
	void SetSize(pixel size_new);
	void SetPos(pixel x, pixel y);
	//������ʾ����ʱ���ֵ�λ�á�(x,y)���������Ͻ���Դ��ڿͻ�ȥ���Ͻǵ����꣬������Ϊ��λ
	//��ǰ���ǣ��������ڴ���GLT_2Dģʽ�¡�
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
