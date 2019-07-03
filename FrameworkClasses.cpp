#include"FrameworkClasses.h"

GLWindow::GLWindow(string caption, int width, int height)
	:width{width},height{height}
{
	_JudgeUnique();

	_MyCreateGLWindow(caption, width, height);
	_InitGLContext();

	ShowWindow(hWnd, SW_SHOW);				// 显示窗口
	SetForegroundWindow(hWnd);					// 略略提高优先级
	SetFocus(hWnd);									// 设置键盘的焦点至此窗口
}

GLWindow::~GLWindow()
{
	if (!wglMakeCurrent(NULL, NULL))	// 我们能否释放DC和RC描述表?
		throw runtime_error{ "Terminating Error: Fail to release HDC or HRC" };
	if (!wglDeleteContext(hRC))				// 我们能否删除RC?
		throw runtime_error{ "Terminating Error: Fail to release HRC" };
	if (!ReleaseDC(hWnd, hDC))				// 我们能否释放 DC?
		throw runtime_error{ "Terminating Error: Fail to release HDC" };
	if (hWnd && !DestroyWindow(hWnd))	// 能否销毁窗口?
		throw runtime_error{"Terminating Error: Fail to release HWND"};
	if (!UnregisterClass("MyWorld", hInstance))	// 能否注销类?
		throw runtime_error{ "Terminating Error: Fail to unregister window class" };
}
void GLWindow::Update()//will not clog
{
	//另外一部分：因为这四个变量代表的是一次性的事件，故只持续一帧
	{
		if (cmd.mouse.EventLButtonDown)
			cmd.mouse.EventLButtonDown = false;
		if (cmd.mouse.EventLButtonUp)
			cmd.mouse.EventLButtonUp = false;
		if (cmd.mouse.EventRButtonDown)
			cmd.mouse.EventRButtonDown = false;
		if (cmd.mouse.EventRButtonUp)
			cmd.mouse.EventRButtonUp = false;
	}

	MSG	msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))			// 若有消息，处理消息
	{
		if (msg.message == WM_QUIT)				// 收到退出消息?
			quit = (MessageBox(hWnd, "你确定要退出吗？", "", MB_OKCANCEL) == IDOK);
		else							// 不是，处理窗口消息
		{
			TranslateMessage(&msg);				// 翻译消息
			DispatchMessage(&msg);				// 发送消息
		}
	}
	//return true;
	//经实验，调用一次此函数大约会消耗10^(-7)s左右，在一帧的时间之下可以忽略不计。
	//不过，当移动窗口、窗口重绘时，消耗的时间会显著上升，估计在前一情况的100倍以上。
	//所幸，这种情况并不常常发生。
}

void GLWindow::_MyRegisterWindowClass()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC	// 移动时重画，并为窗口取得DC
				| CS_GLOBALCLASS;									// 声明此窗口类为app中的全局窗口类
	wc.lpfnWndProc = (WNDPROC)_WndProc;					// WndProc处理消息
	wc.cbClsExtra = 0;													// 无额外窗口数据
	wc.cbWndExtra = 0;												// 无额外窗口数据
	wc.hInstance = hInstance;										// 设置实例
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);					// 装入缺省图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// 装入鼠标指针
	wc.hbrBackground = NULL;										// GL不需要背景
	wc.lpszMenuName = NULL;										// 不需要菜单
	wc.lpszClassName = "MyWorld";									// 设定类名字

	if (!RegisterClass(&wc))					// 尝试注册窗口类
		throw runtime_error("Init Error: Fail to register a window class");
}
void GLWindow::_MyCreateWindow(string caption, int width, int height)
{
	DWORD		dwExStyle;							// 扩展窗口风格
	DWORD		dwStyle;								// 窗口风格

	dwStyle = 0l;
	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// 扩展窗体风格
	dwStyle = WS_OVERLAPPEDWINDOW;									//  窗体风格
	dwExStyle = 0l;

	_MyRegisterWindowClass();
	hWnd = CreateWindowEx(dwExStyle,			// 扩展窗体风格
		"MyWorld",										// 类名字
		caption.c_str(),									// 窗口标题
		dwStyle,											// 选择的窗体属性
		0, 0,												// 窗口位置
		width,											// 计算调整好的窗口宽度
		height,											// 计算调整好的窗口高度
		NULL,												// 无父窗口
		NULL,												// 无菜单
		hInstance,										// 实例
		this);												// 为了实现封装，将this指针传过去，参考：https://zhidao.baidu.com/question/621610399391343372.html

	if (!hWnd)
		throw runtime_error("Init Error: Fail to create the window and fail to get HWND");
}
void GLWindow::_MyCreateGLWindow(string caption, int width, int height)
{
	_MyCreateWindow(caption, width, height);

	PIXELFORMATDESCRIPTOR pfd =			// /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),			// 上述格式描述符的大小
		1,														// 版本号
		PFD_DRAW_TO_WINDOW |						// 格式支持窗口
		PFD_SUPPORT_OPENGL |						// 格式必须支持OpenGL
		PFD_DOUBLEBUFFER,							// 必须支持双缓冲
		PFD_TYPE_RGBA,									// 申请 RGBA 格式
		16,													// 选定色彩深度
		0, 0, 0, 0, 0, 0,										// 忽略的色彩位
		0,														// 无Alpha缓存
		0,														// 忽略Shift Bit
		0,														// 无累加缓存
		0, 0, 0, 0,											// 忽略聚集位
		16,													// 16位 Z-缓存 (深度缓存)
		0,														// 无蒙板缓存
		0,														// 无辅助缓存
		PFD_MAIN_PLANE,								// 主绘图层
		0,														// Reserved
		0, 0, 0												// 忽略层遮罩
	};

	if (!(hDC = GetDC(hWnd)))						// 取得设备描述表了么?
		throw runtime_error("Init Error: Fail to get HDC");
	GLuint PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Windows 找到相应的象素格式了吗?
		throw runtime_error("Init Error: Fail to choose a pixel format");
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))	// 能够设置象素格式么?
		throw runtime_error("Init Error: Fail to set the pixel format");
	if (!(hRC = wglCreateContext(hDC)))			// 能否取得着色描述表?
		throw runtime_error("Init Error: Fail to get an OpenGL HGLRC");
	if (!wglMakeCurrent(hDC, hRC))				// 尝试激活OpenGL渲染上下文
		throw runtime_error("Init Error: Fail to activate the current HGLRC");
	_OnResize(width, height);
	
}
void GLWindow::_InitGLContext()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
void GLWindow::_OnResize(int width, int height)
{
	if (width < 0 || height < 0)
		throw runtime_error{ "a window with non-positive width or height?!!!" };
	glViewport(0, 0, width, height);					// 重置当前的视口
	//投影矩阵什么的不归GLWindow类管
}

void GLWindow::_JudgeUnique()
{
	static int wnd_count{ 0 };
	wnd_count++;
	if (wnd_count > 1) throw runtime_error{ "More than one window created!" };
}

LRESULT CALLBACK
GLWindow::
_MyWndProc(
		UINT	uMsg,							// 窗口的消息
		WPARAM wParam,				// 附加的消息内容
		LPARAM	 lParam)					// 附加的消息内容
{
	switch (uMsg)								// 检查Windows消息
	{
	case WM_ACTIVATE:						// 监视窗口激活消息
		active = !HIWORD(wParam);			//一种比较简洁的写法 检查最小化状态
		return 0;									// 返回消息循环

	case WM_SYSCOMMAND:				// 系统中断命令
		if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)			// 检查系统调用
	case SC_SCREENSAVE:				// 屏保要运行?
	case SC_MONITORPOWER:			// 显示器要进入节电模式?
		return 0;									// 阻止发生/返回
		break;

	case WM_KEYDOWN:						// 有键按下么?
		cmd.keys[wParam] = true;					// 如果是，设为TRUE
		return 0;									// 返回
	case WM_KEYUP:							// 有键弹起么?
		cmd.keys[wParam] = false;					// 如果是，设为FALSE
		return 0;									// 返回
	case WM_MOUSEMOVE:					//鼠标消息
		cmd.mouse.x = LOWORD(lParam);		//获得鼠标位置
		cmd.mouse.y = HIWORD(lParam);
		return 0;
	case WM_LBUTTONDOWN:				//其他鼠标消息：左右键
		cmd.mouse.LButton = true;
		//cmd.mouse.EventLButtonDown = true;
		return 0;
	case WM_LBUTTONUP:
		cmd.mouse.LButton = false;
		cmd.mouse.EventLButtonUp = true;
		return 0;
	case WM_RBUTTONDOWN:
		cmd.mouse.RButton = true;
		cmd.mouse.EventRButtonDown = true;
		return 0;
	case WM_RBUTTONUP:
		cmd.mouse.RButton = false;
		cmd.mouse.EventRButtonUp = true;
		return 0;

	case WM_SIZE:						// 调整OpenGL窗口大小
		width = LOWORD(lParam);// LoWord=Width,HiWord=Height
		height = HIWORD(lParam);
		_OnResize(width,height);		
		return 0;							// 返回

	case WM_CLOSE:					// 收到Close消息?
											// extern Map map;
											// map.Save();
		PostQuitMessage(0);			// 发出退出消息
		return 0;							// 返回
		break;
		
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);// 向 DefWindowProc传递所有未处理的消息
}

LRESULT CALLBACK
_WndProc(HWND hWnd,				// 窗口的句柄
	UINT	uMsg,							// 窗口的消息
	WPARAM wParam,				// 附加的消息内容
	LPARAM	 lParam)
{
	GLWindow* pthis{ nullptr };
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pthis = static_cast<GLWindow*>(lpcs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pthis));
	}
	else
	{
		pthis = reinterpret_cast<GLWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pthis)
			return pthis->_MyWndProc(uMsg, wParam, lParam);
		else
			//throw runtime_error{ "Window Class Encapsulation Error: _WndProc and _MyWndProc" };
			return DefWindowProc(hWnd, uMsg, wParam, lParam);// 向 DefWindowProc传递所有未处理的消息
	}
}

void GLWindow::SetCursorPos(int x,int y)
{
	POINT p;
	p.x = 0, p.y = 0;
	ScreenToClient(hWnd, &p);

	::SetCursorPos(x - p.x, y - p.y);
}
void GLWindow::ShowCursor(bool show)
{
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	if (show^show_cursor)//^是异或
	{
		::ShowCursor(show);//注意作用域
		show_cursor = show;
	}
	//这是为了处理showcursor的计数问题：每一次ShowCursor会让指针的显示计数加减一，
	//大于零时显示鼠标，小于等于0时隐藏鼠标，而不是简单的机制。
}
void GLWindow::HideCursor()
{
	ShowCursor(false);
}