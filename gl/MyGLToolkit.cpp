#include"MyGLToolkit.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include"gl\stb_image.h"
//������Ⱦ��غ���
/*
PS:ֻ����Ⱦascii�ַ���������Ⱦ�����֣������������ֵ����塢λ�á���ɫ���ֺ�/��С����λ�����أ�
����������������ֺŵ�ʱ�򣬻����³�ʼ�����壬�Ƚ�����
��ײ�����һ��gltTextRenderer���󡣵�ͨ��gltGetTextRenderer����+��̬�ֲ�������ȫ�ֶ���������ˣ�
��ͨ��һϵ�к�����װ����opengl�⺯���ķ��
*/

void gltTextRenderer::Update()//���³�ʼ��
{
	HDC hDC = //GetWindowDC(GetActiveWindow());
		wglGetCurrentDC();
	//HFONT	hFont;						// ������
	hFont = CreateFont(-(int)size,			// ����߶�
		0,									// ������
		0,									// �������ת�Ƕ� Angle Of Escapement
		0,									// ������ߵ���ת�Ƕ�Orientation Angle
		0,									// ���������
		FALSE,							// �Ƿ�ʹ��б��
		FALSE,							// �Ƿ�ʹ���»���
		FALSE,							// �Ƿ�ʹ��ɾ����
		ANSI_CHARSET,					// �����ַ���
		OUT_TT_PRECIS,					// �������
		CLIP_DEFAULT_PRECIS,			// �ü�����
		ANTIALIASED_QUALITY,			// �������
		FF_DONTCARE | DEFAULT_PITCH,		// Family And Pitch
		font.c_str());						// ��������

	SelectObject(hDC, hFont);	// ѡ��������Ҫ������
	_char_list_entry = glGenLists(256);					// ����256����ʾ�б�
	wglUseFontBitmaps(hDC, 0, 256, _char_list_entry);		// ����256����ʾ�б����ƴ�ASCII����֮��Ӧ�ķ������ַ�
	GetTextMetrics(hDC, &tm);
}
gltTextRenderer::gltTextRenderer()
{
	color[0] = 1.0f, color[1] = 1.0f, color[2] = 1.0f;
	size = std_size;
	pos[0] = 0, pos[1] = 0;
	Update();
}
gltTextRenderer::gltTextRenderer(string font, unsigned int size)
	:font{ font }, size{ size } 
{
	color[0] = 1.0f, color[1] = 1.0f, color[2] = 1.0f;
	size = std_size;
	pos[0] = 0, pos[1] = 0;
	Update();
}
gltTextRenderer::~gltTextRenderer()
	{
		glDeleteLists(_char_list_entry, 256);
	}

gltTextRenderer* gltTextRenderer::pPrevObj;
//���ĺ���������
void gltTextRenderer::RenderText(string text)
{
	if (pPrevObj != this)//��������Ҫ�л�����ʱ
	{
		SelectObject(wglGetCurrentDC(), hFont);// ѡ��������Ҫ������
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 256, _char_list_entry);
		//����������������������������������ʽ�Ż������ı�
		pPrevObj = this;
	}
	
	//����Ƿ���GLT_2Dģʽ�¡������ǣ������renderʱ�׳�����
	if(gltGetProjectionMode()!=GLT_2D) throw runtime_error{ "error: you can't render text under GLT_3D mode" };
	glColor3f(color[0], color[1], color[2]);
	glPushAttrib(GL_LIST_BIT);				// ����ʾ�б�����ѹ�����Զ�ջ

	SelectObject(GetWindowDC(GetActiveWindow()), hFont);

	if(align_mode==GLT_DEFAULT)//���ݲ�ͬ�Ķ���ģʽ����λ�á�ע��ڶ��������л�+size������һ��Сϸ�ڡ���һ�Ծ�֪���ˡ�
		glRasterPos2f(pos[0], pos[1] + size);
	else if (align_mode == GLT_MID)
		glRasterPos2f(pos[0] - tm.tmAveCharWidth*text.size() / 2, pos[1] + size - tm.tmHeight / 2);
	glListBase(_char_list_entry);							// ������ʾ�б�Ļ���ֵ
	glCallLists(strlen(text.c_str()), GL_UNSIGNED_BYTE, text.c_str());	// ������ʾ�б�����ַ���
	glPopAttrib();								// �������Զ�ջ
}
//set��get����
void gltTextRenderer::SetFont(string font_new) {
		font = font_new;
		Update();
	}
void gltTextRenderer::SetColor(float R, float G, float B) { color[0] = R, color[1] = G, color[2] = B; }
void gltTextRenderer::SetSize(pixel size_new) {
		size = size_new;
		Update();
	}
void gltTextRenderer::SetPos(pixel x, pixel y) {
		pos[0] = x, pos[1] = y;
	}
//������ʾ����ʱ���ֵ�λ�á�(x,y)���������Ͻ���Դ��ڿͻ�ȥ���Ͻǵ����꣬������Ϊ��λ
//��ǰ���ǣ��������ڴ���GLT_2Dģʽ�¡�
void gltTextRenderer::SetAlignMode(unsigned int mode_new) { align_mode = mode_new; }
int gltTextRenderer::GetSize() { return size; }
gltTextRenderer& gltGetTextRenderer()
{
	static gltTextRenderer renderer;
	return renderer;
}
void gltTextColor(float R, float G, float B)
{
	gltGetTextRenderer().SetColor(R, G, B);
}
void gltTextPos(pixel x, pixel y)
{
	gltGetTextRenderer().SetPos(x, y);
}
void gltTextFont(string font_new)
{
	gltGetTextRenderer().SetFont(font_new);
}
void gltTextSize(pixel size_new)
{
	gltGetTextRenderer().SetSize(size_new);
}
void gltTextAlignMode(unsigned int mode_new)
{
	gltGetTextRenderer().SetAlignMode(mode_new);
}
void gltRenderText(string text)
{
	gltGetTextRenderer().RenderText(text);
}
void gltRenderText(string text, pixel x, pixel y)
{
	gltGetTextRenderer().SetPos(x, y);
	gltGetTextRenderer().RenderText(text);
}

class gltTextureHelper
{
private:
	vector<unsigned char*> texture_data;
public:
	gltTextureHelper()
	{
		//��ʼ��������ز���
		stbi_set_flip_vertically_on_load(true);//��תһ�£�ʹ��stbi����opengl���Ӧ�����ߵı�׼��ͬ��


	}
	void AddTextureData(unsigned char* tex_data) { texture_data.push_back(tex_data); }
	~gltTextureHelper()
	{
		for (unsigned char* data : texture_data)
			stbi_image_free(data);
	}

};
gltTextureHelper& gltGetTextureHelper()
{
	static gltTextureHelper texture_helper;
	return texture_helper;
}
HTEX gltLoadTexture(string filepath,unsigned int* pw,unsigned int* ph)
{
	gltGetTextureHelper();

	int width, height, nrChannels;
	unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);//����stbi��ʵ�ʶ�ȡ����
	if (!data) throw runtime_error{ "gltLoadTexture failure!" };//����ȡʧ�ܣ��׳�����
	if (pw&&ph)
	{
		*pw = width;
		*ph = height;
	}//��ͼƬ�Ŀ�߷��س�ȥ

	HTEX hTex;
	glGenTextures(1, &hTex);
	glBindTexture(GL_TEXTURE_2D, hTex);
	if (nrChannels == 3)//û��alphaͨ��
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (nrChannels == 4)//��alphaͨ��
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// ���ط�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	gltGetTextureHelper().AddTextureData(data);//��֪gltTextureHelper���Ա�������ʱ�Զ��ͷ���Դ

	return hTex;
}

void gltSetProjectionMode(unsigned int mode, int wnd_width, int wnd_height)
{
	if (mode == GLT_2D)
	{
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);		// ѡ��ͶӰ����
		glLoadIdentity();							// ����ͶӰ����
		glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f);

		glMatrixMode(GL_MODELVIEW);						// ѡ��ģ�͹۲����
		glLoadIdentity();							// ����ģ�͹۲����
		glScalef(2 / (GLfloat)wnd_width, -2 / (GLfloat)wnd_height, 1.0f);
		glTranslatef((GLfloat)-wnd_width / 2, (GLfloat)-wnd_height / 2, 0.0f);
		//����Ļ����� ( (-1.0f,-1.0f) , (-1.0f,1.0f) ) �����Ͻ�-���½ǣ�ӳ��Ϊ
		//( (0,0) , (w,h) ) (���Ͻ�-���½�)����w��h�Ǵ���ͻ���������Ϊ��λ�ĳ���

		gltGetProjectionMode() = GLT_2D;
	}
	else if (mode == GLT_3D)
	{
		if (wnd_width < 0)
			throw runtime_error{ "wnd_width/height needed when setting 3D projection mode!" };
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����
		glLoadIdentity();							// ����ͶӰ����
		gluPerspective(45.0f, (GLfloat)wnd_width / (GLfloat)wnd_height, 0.1f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gltGetProjectionMode() = GLT_3D;
	}
}
unsigned int & gltGetProjectionMode()
{
	static unsigned int mode{GLT_DEFAULT};
	return mode;
}

void gltBeginRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// �����Ļ����Ȼ���
}
void gltEndRender()
{
	SwapBuffers(wglGetCurrentDC());
}

void gltShowLoadingPage()
{
	gltBeginRender();

	gltTextPos(500, 300);
	gltRenderText("Loading...");

	gltEndRender();
}