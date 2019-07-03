#include"WidgetClasses.h"



GWidget::GWidget()
{
	explicit_proj_mode_setting = false;
	area.left = 100;
	area.right = 180;
	area.top = 100;
	area.bottom = 130;//default setting
}
void GWidget::ExplicitProjectionModeSetting(bool b) { explicit_proj_mode_setting = b; }
void GWidget::SetPos(pixel x_new, pixel y_new)
{
	area.right += x_new - area.left;
	area.left = x_new;
	area.bottom += y_new - area.top;
	area.top = y_new;
}
void GWidget::SetColor(float R_new, float G_new, float B_new, float A_new)
{
	color_R = R_new;
	color_G = G_new;
	color_B = B_new;
	if (A_new > 0.0f)
		color_A = A_new;
}
void GWidget::SetTransparancy(float transparancy)
{
	if (transparancy > 1.0f) transparancy = 1.0f;
	if (transparancy < 0.0f) transparancy = 0.0f;
	color_A = 1.0f - transparancy;
}
void GWidget::SetSize(pixel width, pixel height)
{
	area.right = area.left + width;
	area.bottom = area.top + height;
}
void GWidget::Render()
{
	if (explicit_proj_mode_setting)
		gltSetProjectionMode(GLT_2D);

	glEnable(GL_BLEND);
	glColor4f(color_R, color_G, color_B, color_A);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	glVertex2f((float)area.left, (float)area.top);
	glVertex2f((float)area.left, (float)area.bottom);
	glVertex2f((float)area.right, (float)area.bottom);
	glVertex2f((float)area.right, (float)area.top);
	glEnd();

	gltRenderText(string{ "widget" }, area.left + 5, area.top);
}

GRectangle::GRectangle(pixel left, pixel top, pixel right, pixel bottom)
{
	area.top = top;
	area.left = left;
	area.right = right;
	area.bottom = bottom;
}
void GRectangle::Render()
{
	if (explicit_proj_mode_setting)
		gltSetProjectionMode(GLT_2D);

	glEnable(GL_BLEND);
	glColor4f(color_R, color_G, color_B, color_A);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	glVertex2f((float)area.left, (float)area.top);
	glVertex2f((float)area.left, (float)area.bottom);
	glVertex2f((float)area.right, (float)area.bottom);
	glVertex2f((float)area.right, (float)area.top);
	glEnd();
	glDisable(GL_BLEND);
}

GBackground::GBackground(string filepath)
{
	unsigned int w, h;
	background = gltLoadTexture(filepath, &w, &h);
	area.left = area.top = 0;
	SetSize(w, h);
}
void GBackground::SetSize(pixel width, pixel height)
{
	area.right = area.left + width;
	area.bottom = area.top + height;
}
void GBackground::Render()
{
	if (explicit_proj_mode_setting)
		gltSetProjectionMode(GLT_2D);

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, background);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f((float)area.left, (float)area.top);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f((float)area.left, (float)area.bottom);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f((float)area.right, (float)area.bottom);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f((float)area.right, (float)area.top);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

vector<gltTextRenderer> GText::renderers;
GText::GText()
{
	if (renderers.empty())
		GText_static();
	SetColor(1.0f, 1.0f, 1.0f);
}
GText::GText(string text, unsigned int mode)
	:text{ text }, mode{ mode }
{
	if (renderers.empty())
		GText_static();
	SetColor(1.0f, 1.0f, 1.0f);
}
//the constructive function of the static members
void GText::GText_static()
{
	AddMode("Comic Sans MS", 20);
	AddMode("Comic Sans MS", 50);
	AddMode("Baskerville Old Face", 30);
	AddMode("Harrington", 20);
	AddMode("Harrington", 50);
	AddMode("Papyrus", 50);
}
void GText::AddMode(string font, unsigned int size)
{
	renderers.push_back(gltTextRenderer(font, size));
}
void GText::SetText(string text_new) { text = text_new; }
void GText::SetMode(unsigned int mode_new)
{
	if (mode_new >= renderers.size())
		throw runtime_error{ "class GText: unknown mode!" };
	mode = mode_new;
}
//set和get函数
/*void SetFont(string font_new) {
font = font_new;
}
void SetSize(unsigned int size_new) { size = size_new; }*/
//设置显示文字时文字的位置。(x,y)是文字左上角相对窗口客户去左上角的坐标，以像素为单位
//但前提是，整个窗口处于GLT_2D模式下。
void GText::SetAlignMode(unsigned int mode_new) { align_mode = mode_new; }
void GText::Render()
{
	gltTextRenderer& r = renderers[mode];
	r.SetColor(color_R, color_G, color_B);
	//gltTextSize(size);
	//gltTextFont(font);
	r.SetAlignMode(align_mode);
	r.SetPos(area.left, area.top);
	r.RenderText(text);
}


string GButton::_GetKind() { return "GButton"; }
//set/get函数
void GButton::SetCaption(string caption_new) { caption = caption_new; }
void GButton::SetFunction(string type, pfunc func)
{
	if (type == string{ "cursor_passing" })
		func_cursor_passing = func;
	else if (type == string{ "cursor_clicking" })
		func_cursor_clicking = func;
	else if (type == string{ "mouse_down" })
		func_mouse_down = func;
	else if (type == string{ "mouse_up" })
		func_mouse_up = func;
	else
		throw runtime_error{ "unknown callback function type!" };
}
//判断一个点是不是在button范围内
bool GButton::WinthinBoundary(pixel x, pixel y)
{
	return x >= area.left && x <= area.right && y >= area.top && y <= area.bottom;
}
//回调函数/事件函数
string GButton::OnCursorPassing(void* param)
{
	is_cursor_passing = true;
	string result{ nullstr };
	if (func_cursor_passing)
		result = func_cursor_passing(param);
	return result;
}
string GButton::OnCursorClicking(void* param)
{
	is_cursor_clicking = true;
	string result{ nullstr };
	if (func_cursor_clicking)
		result = func_cursor_clicking(param);
	return result;
}
string GButton::OnMouseDown(void* param)//真正触发事件的函数
{
	string result{ nullstr };
	if (func_mouse_down)
	{
		result = func_mouse_down(param);
		return result;
	}
	else//事件触发时，默认返回nullstr
		return nullstr;
}
string GButton::OnMouseUp(void* param)//真正触发事件的函数
{
	string result{ nullstr };
	if (func_mouse_up)
	{
		result = func_mouse_up(param);
		return result;
	}
	else//事件触发时，默认返回按钮名
		return caption;
}
void GButton::Render()
{
	if (explicit_proj_mode_setting)
		gltSetProjectionMode(GLT_2D);

	glEnable(GL_BLEND);
	glColor4f(color_R, color_G, color_B, color_A
		- (is_cursor_passing ? 0.1f : 0.0f) - (is_cursor_clicking ? 0.3f : 0.0f));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	is_cursor_passing = false;
	is_cursor_clicking = false;

	glBegin(GL_QUADS);
	glVertex2f((float)area.left, (float)area.top);
	glVertex2f((float)area.left, (float)area.bottom);
	glVertex2f((float)area.right, (float)area.bottom);
	glVertex2f((float)area.right, (float)area.top);
	glEnd();

	gltTextAlignMode(GLT_MID);
	gltTextColor(1.0f, 1.0f, 1.0f);
	gltRenderText(caption, (area.left + area.right) / 2, (area.top + area.bottom) / 2);
}


//添加控件的函数。注意：添加进来的只是控件的指针，不是拷贝
void GContainer::AttachWidget(GWidget* w)
{
	widgets.push_back(w);
	if (w->_GetKind() == string{ "GButton" })
		buttons.push_back((GButton*)w);
}
//作为控件渲染
void GContainer::Render()
{
	for (GWidget* pw : widgets)
		pw->Render();
}
//作为窗体运行
string GContainer::SingleLoop()//只循环一次的Run
{
	if (!(pWnd->Quit()))//经包装，一种简洁的消息循环
	{
		pWnd->Update();
		gltBeginRender();

		//划重点！！！！！看好了！！！！
		pixel x = pWnd->GetConsole().mouse.x;
		pixel y = pWnd->GetConsole().mouse.y;
		Mouse& m = pWnd->GetConsole().mouse;
		string result{ nullstr };

		for (GButton* b : buttons)
		{
			if (b->WinthinBoundary(x, y))
			{
				if (m.LButton)//鼠标按下了
					result = b->OnCursorClicking(this);
				else
					result = b->OnCursorPassing(this);
				if (m.EventRButtonDown)//鼠标从按键上松开的时候触发事件，按钮被真正触发
					result = b->OnMouseDown(this);
				else if (m.EventRButtonUp)
					result = b->OnMouseUp(this);
			}
		}
		for (GWidget* pw : widgets)
			pw->Render();
		gltEndRender();
		return result;
	}
	else return "quit";
}
string GContainer::Run()
{
	while (!(pWnd->Quit()))//经包装，一种简洁的消息循环
	{
		pWnd->Update();
		gltBeginRender();

		//划重点！！！！！看好了！！！！
		pixel x = pWnd->GetConsole().mouse.x;
		pixel y = pWnd->GetConsole().mouse.y;
		Mouse& m = pWnd->GetConsole().mouse;
		string result{ nullstr };

		for (GButton* b : buttons)
		{
			if (b->WinthinBoundary(x, y))
			{
				if (m.LButton)//鼠标按下了
					result = b->OnCursorClicking(this);
				else
					result = b->OnCursorPassing(this);
				if (m.EventLButtonDown)//鼠标从按键上松开的时候触发事件，按钮被真正触发
					result = b->OnMouseDown(this);
				else if (m.EventLButtonUp)
					result = b->OnMouseUp(this);
			}
		}
		for (GWidget* pw : widgets)
			pw->Render();
		gltEndRender();

		if (result != nullstr) return result;
	}
	return "quit";
}