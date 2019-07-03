#include"MyGLToolkit.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include"gl\stb_image.h"
//文字渲染相关函数
/*
PS:只能渲染ascii字符，不能渲染中文字；可以设置文字的字体、位置、颜色、字号/大小（单位：像素）
在重新设置字体和字号的时候，会重新初始化字体，比较慢。
最底层用了一个gltTextRenderer对象。但通过gltGetTextRenderer函数+静态局部变量把全局对象藏起来了，
并通过一系列函数包装成了opengl库函数的风格。
*/

void gltTextRenderer::Update()//重新初始化
{
	HDC hDC = //GetWindowDC(GetActiveWindow());
		wglGetCurrentDC();
	//HFONT	hFont;						// 字体句柄
	hFont = CreateFont(-(int)size,			// 字体高度
		0,									// 字体宽度
		0,									// 字体的旋转角度 Angle Of Escapement
		0,									// 字体底线的旋转角度Orientation Angle
		0,									// 字体的重量
		FALSE,							// 是否使用斜体
		FALSE,							// 是否使用下划线
		FALSE,							// 是否使用删除线
		ANSI_CHARSET,					// 设置字符集
		OUT_TT_PRECIS,					// 输出精度
		CLIP_DEFAULT_PRECIS,			// 裁剪精度
		ANTIALIASED_QUALITY,			// 输出质量
		FF_DONTCARE | DEFAULT_PITCH,		// Family And Pitch
		font.c_str());						// 字体名称

	SelectObject(hDC, hFont);	// 选择我们需要的字体
	_char_list_entry = glGenLists(256);					// 创建256个显示列表
	wglUseFontBitmaps(hDC, 0, 256, _char_list_entry);		// 创建256个显示列表，绘制从ASCII码与之对应的非特殊字符
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
//核心函数！！！
void gltTextRenderer::RenderText(string text)
{
	if (pPrevObj != this)//当我们需要切换字体时
	{
		SelectObject(wglGetCurrentDC(), hFont);// 选择我们需要的字体
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 256, _char_list_entry);
		//当你用了这两个函数，画出来的文字样式才会真正改变
		pPrevObj = this;
	}
	
	//检测是否在GLT_2D模式下。若不是，则会在render时抛出错误。
	if(gltGetProjectionMode()!=GLT_2D) throw runtime_error{ "error: you can't render text under GLT_3D mode" };
	glColor3f(color[0], color[1], color[2]);
	glPushAttrib(GL_LIST_BIT);				// 把显示列表属性压入属性堆栈

	SelectObject(GetWindowDC(GetActiveWindow()), hFont);

	if(align_mode==GLT_DEFAULT)//根据不同的对齐模式设置位置。注意第二个坐标中会+size，这是一个小细节。试一试就知道了。
		glRasterPos2f(pos[0], pos[1] + size);
	else if (align_mode == GLT_MID)
		glRasterPos2f(pos[0] - tm.tmAveCharWidth*text.size() / 2, pos[1] + size - tm.tmHeight / 2);
	glListBase(_char_list_entry);							// 设置显示列表的基础值
	glCallLists(strlen(text.c_str()), GL_UNSIGNED_BYTE, text.c_str());	// 调用显示列表绘制字符串
	glPopAttrib();								// 弹出属性堆栈
}
//set和get函数
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
//设置显示文字时文字的位置。(x,y)是文字左上角相对窗口客户去左上角的坐标，以像素为单位
//但前提是，整个窗口处于GLT_2D模式下。
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
		//初始化纹理相关参数
		stbi_set_flip_vertically_on_load(true);//翻转一下，使得stbi库与opengl相对应（二者的标准不同）


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
	unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);//调用stbi库实际读取数据
	if (!data) throw runtime_error{ "gltLoadTexture failure!" };//若读取失败，抛出错误
	if (pw&&ph)
	{
		*pw = width;
		*ph = height;
	}//将图片的宽高返回出去

	HTEX hTex;
	glGenTextures(1, &hTex);
	glBindTexture(GL_TEXTURE_2D, hTex);
	if (nrChannels == 3)//没有alpha通道
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (nrChannels == 4)//有alpha通道
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// 像素风
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	gltGetTextureHelper().AddTextureData(data);//告知gltTextureHelper，以便程序结束时自动释放资源

	return hTex;
}

void gltSetProjectionMode(unsigned int mode, int wnd_width, int wnd_height)
{
	if (mode == GLT_2D)
	{
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);		// 选择投影矩阵
		glLoadIdentity();							// 重置投影矩阵
		glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f);

		glMatrixMode(GL_MODELVIEW);						// 选择模型观察矩阵
		glLoadIdentity();							// 重置模型观察矩阵
		glScalef(2 / (GLfloat)wnd_width, -2 / (GLfloat)wnd_height, 1.0f);
		glTranslatef((GLfloat)-wnd_width / 2, (GLfloat)-wnd_height / 2, 0.0f);
		//把屏幕坐标从 ( (-1.0f,-1.0f) , (-1.0f,1.0f) ) （左上角-右下角）映射为
		//( (0,0) , (w,h) ) (左上角-右下角)其中w和h是窗体客户区以坐标为单位的长宽

		gltGetProjectionMode() = GLT_2D;
	}
	else if (mode == GLT_3D)
	{
		if (wnd_width < 0)
			throw runtime_error{ "wnd_width/height needed when setting 3D projection mode!" };
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);						// 选择投影矩阵
		glLoadIdentity();							// 重置投影矩阵
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// 清除屏幕及深度缓存
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