#include"FrameworkClasses.h"

GLWindow::GLWindow(string caption, int width, int height)
	:width{width},height{height}
{
	_JudgeUnique();

	_MyCreateGLWindow(caption, width, height);
	_InitGLContext();

	ShowWindow(hWnd, SW_SHOW);				// ��ʾ����
	SetForegroundWindow(hWnd);					// ����������ȼ�
	SetFocus(hWnd);									// ���ü��̵Ľ������˴���
}

GLWindow::~GLWindow()
{
	if (!wglMakeCurrent(NULL, NULL))	// �����ܷ��ͷ�DC��RC������?
		throw runtime_error{ "Terminating Error: Fail to release HDC or HRC" };
	if (!wglDeleteContext(hRC))				// �����ܷ�ɾ��RC?
		throw runtime_error{ "Terminating Error: Fail to release HRC" };
	if (!ReleaseDC(hWnd, hDC))				// �����ܷ��ͷ� DC?
		throw runtime_error{ "Terminating Error: Fail to release HDC" };
	if (hWnd && !DestroyWindow(hWnd))	// �ܷ����ٴ���?
		throw runtime_error{"Terminating Error: Fail to release HWND"};
	if (!UnregisterClass("MyWorld", hInstance))	// �ܷ�ע����?
		throw runtime_error{ "Terminating Error: Fail to unregister window class" };
}
void GLWindow::Update()//will not clog
{
	//����һ���֣���Ϊ���ĸ������������һ���Ե��¼�����ֻ����һ֡
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
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))			// ������Ϣ��������Ϣ
	{
		if (msg.message == WM_QUIT)				// �յ��˳���Ϣ?
			quit = (MessageBox(hWnd, "��ȷ��Ҫ�˳���", "", MB_OKCANCEL) == IDOK);
		else							// ���ǣ���������Ϣ
		{
			TranslateMessage(&msg);				// ������Ϣ
			DispatchMessage(&msg);				// ������Ϣ
		}
	}
	//return true;
	//��ʵ�飬����һ�δ˺�����Լ������10^(-7)s���ң���һ֡��ʱ��֮�¿��Ժ��Բ��ơ�
	//���������ƶ����ڡ������ػ�ʱ�����ĵ�ʱ�������������������ǰһ�����100�����ϡ�
	//���ң����������������������
}

void GLWindow::_MyRegisterWindowClass()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC	// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
				| CS_GLOBALCLASS;									// �����˴�����Ϊapp�е�ȫ�ִ�����
	wc.lpfnWndProc = (WNDPROC)_WndProc;					// WndProc������Ϣ
	wc.cbClsExtra = 0;													// �޶��ⴰ������
	wc.cbWndExtra = 0;												// �޶��ⴰ������
	wc.hInstance = hInstance;										// ����ʵ��
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);					// װ��ȱʡͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				// װ�����ָ��
	wc.hbrBackground = NULL;										// GL����Ҫ����
	wc.lpszMenuName = NULL;										// ����Ҫ�˵�
	wc.lpszClassName = "MyWorld";									// �趨������

	if (!RegisterClass(&wc))					// ����ע�ᴰ����
		throw runtime_error("Init Error: Fail to register a window class");
}
void GLWindow::_MyCreateWindow(string caption, int width, int height)
{
	DWORD		dwExStyle;							// ��չ���ڷ��
	DWORD		dwStyle;								// ���ڷ��

	dwStyle = 0l;
	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// ��չ������
	dwStyle = WS_OVERLAPPEDWINDOW;									//  ������
	dwExStyle = 0l;

	_MyRegisterWindowClass();
	hWnd = CreateWindowEx(dwExStyle,			// ��չ������
		"MyWorld",										// ������
		caption.c_str(),									// ���ڱ���
		dwStyle,											// ѡ��Ĵ�������
		0, 0,												// ����λ��
		width,											// ��������õĴ��ڿ��
		height,											// ��������õĴ��ڸ߶�
		NULL,												// �޸�����
		NULL,												// �޲˵�
		hInstance,										// ʵ��
		this);												// Ϊ��ʵ�ַ�װ����thisָ�봫��ȥ���ο���https://zhidao.baidu.com/question/621610399391343372.html

	if (!hWnd)
		throw runtime_error("Init Error: Fail to create the window and fail to get HWND");
}
void GLWindow::_MyCreateGLWindow(string caption, int width, int height)
{
	_MyCreateWindow(caption, width, height);

	PIXELFORMATDESCRIPTOR pfd =			// /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),			// ������ʽ�������Ĵ�С
		1,														// �汾��
		PFD_DRAW_TO_WINDOW |						// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,							// ����֧��˫����
		PFD_TYPE_RGBA,									// ���� RGBA ��ʽ
		16,													// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,										// ���Ե�ɫ��λ
		0,														// ��Alpha����
		0,														// ����Shift Bit
		0,														// ���ۼӻ���
		0, 0, 0, 0,											// ���Ծۼ�λ
		16,													// 16λ Z-���� (��Ȼ���)
		0,														// ���ɰ建��
		0,														// �޸�������
		PFD_MAIN_PLANE,								// ����ͼ��
		0,														// Reserved
		0, 0, 0												// ���Բ�����
	};

	if (!(hDC = GetDC(hWnd)))						// ȡ���豸��������ô?
		throw runtime_error("Init Error: Fail to get HDC");
	GLuint PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Windows �ҵ���Ӧ�����ظ�ʽ����?
		throw runtime_error("Init Error: Fail to choose a pixel format");
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))	// �ܹ��������ظ�ʽô?
		throw runtime_error("Init Error: Fail to set the pixel format");
	if (!(hRC = wglCreateContext(hDC)))			// �ܷ�ȡ����ɫ������?
		throw runtime_error("Init Error: Fail to get an OpenGL HGLRC");
	if (!wglMakeCurrent(hDC, hRC))				// ���Լ���OpenGL��Ⱦ������
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
	glViewport(0, 0, width, height);					// ���õ�ǰ���ӿ�
	//ͶӰ����ʲô�Ĳ���GLWindow���
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
		UINT	uMsg,							// ���ڵ���Ϣ
		WPARAM wParam,				// ���ӵ���Ϣ����
		LPARAM	 lParam)					// ���ӵ���Ϣ����
{
	switch (uMsg)								// ���Windows��Ϣ
	{
	case WM_ACTIVATE:						// ���Ӵ��ڼ�����Ϣ
		active = !HIWORD(wParam);			//һ�ֱȽϼ���д�� �����С��״̬
		return 0;									// ������Ϣѭ��

	case WM_SYSCOMMAND:				// ϵͳ�ж�����
		if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)			// ���ϵͳ����
	case SC_SCREENSAVE:				// ����Ҫ����?
	case SC_MONITORPOWER:			// ��ʾ��Ҫ����ڵ�ģʽ?
		return 0;									// ��ֹ����/����
		break;

	case WM_KEYDOWN:						// �м�����ô?
		cmd.keys[wParam] = true;					// ����ǣ���ΪTRUE
		return 0;									// ����
	case WM_KEYUP:							// �м�����ô?
		cmd.keys[wParam] = false;					// ����ǣ���ΪFALSE
		return 0;									// ����
	case WM_MOUSEMOVE:					//�����Ϣ
		cmd.mouse.x = LOWORD(lParam);		//������λ��
		cmd.mouse.y = HIWORD(lParam);
		return 0;
	case WM_LBUTTONDOWN:				//���������Ϣ�����Ҽ�
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

	case WM_SIZE:						// ����OpenGL���ڴ�С
		width = LOWORD(lParam);// LoWord=Width,HiWord=Height
		height = HIWORD(lParam);
		_OnResize(width,height);		
		return 0;							// ����

	case WM_CLOSE:					// �յ�Close��Ϣ?
											// extern Map map;
											// map.Save();
		PostQuitMessage(0);			// �����˳���Ϣ
		return 0;							// ����
		break;
		
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);// �� DefWindowProc��������δ�������Ϣ
}

LRESULT CALLBACK
_WndProc(HWND hWnd,				// ���ڵľ��
	UINT	uMsg,							// ���ڵ���Ϣ
	WPARAM wParam,				// ���ӵ���Ϣ����
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
			return DefWindowProc(hWnd, uMsg, wParam, lParam);// �� DefWindowProc��������δ�������Ϣ
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
	if (show^show_cursor)//^�����
	{
		::ShowCursor(show);//ע��������
		show_cursor = show;
	}
	//����Ϊ�˴���showcursor�ļ������⣺ÿһ��ShowCursor����ָ�����ʾ�����Ӽ�һ��
	//������ʱ��ʾ��꣬С�ڵ���0ʱ������꣬�����Ǽ򵥵Ļ��ơ�
}
void GLWindow::HideCursor()
{
	ShowCursor(false);
}