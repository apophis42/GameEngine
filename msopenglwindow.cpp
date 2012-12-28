#ifdef _WIN32
#include <windows.h>
#endif

#include "glee/Glee.h"
#include "msopenglwindow.h"
#include "inputinterface.h"

MSOpenGLWindow::MSOpenGLWindow(HINSTANCE hInstance):
m_hinstance(hInstance)
{
	m_input = new InputInterface();
}

MSOpenGLWindow::~MSOpenGLWindow()
{
	delete m_input;
}

bool MSOpenGLWindow::create(int width, int height, int bpp, bool fullscreen, wchar_t* name)
{
	DWORD dwExStyle; //Windows Extended Style
	DWORD dwStyle; //Windows Style

	m_name = name;
	m_isFullscreen = fullscreen;

	//Set size of the window
	m_windowRect.left = (long)0;
	m_windowRect.right = (long)width;
	m_windowRect.top = (long)0;
	m_windowRect.bottom = (long)height;

	//Fill out window class structure
	m_windowClass.cbSize = sizeof(WNDCLASSEX);
	m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
	m_windowClass.lpfnWndProc = MSOpenGLWindow::StaticWndProc; //set the event handler
	m_windowClass.cbClsExtra = 0;
	m_windowClass.cbWndExtra = 0;
	m_windowClass.hInstance = m_hinstance;
	m_windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //default icon
	m_windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	m_windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); //default arrow
	m_windowClass.hbrBackground = NULL;
	m_windowClass.lpszMenuName = NULL;
	m_windowClass.lpszClassName = L"GLClass";

	//Register the windows class
	if(!RegisterClassEx(&m_windowClass))
	{
		return false;
	}

	if(m_isFullscreen) //If fullscreen, change the display mode
	{
		DEVMODE dmScreenSettings; //device mode

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);

		dmScreenSettings.dmPelsWidth = width; //screen width;
		dmScreenSettings.dmPelsHeight = height; //screen height;
		dmScreenSettings.dmBitsPerPel = bpp; //bits per pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			//setting display mode failed, switch to windowed
			MessageBox(NULL, L"Display mode failed", NULL, MB_OK);
			m_isFullscreen = false;
		}
	}

	if(m_isFullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}

	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&m_windowRect, dwStyle, false, dwExStyle); //Adjust window to true requested size

	//Class is registered, create window
	m_hwnd = CreateWindowEx(NULL, //extended style
		L"GLClass", //class name
		m_name, //name
		dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0,
		m_windowRect.right - m_windowRect.left, //width
		m_windowRect.bottom - m_windowRect.top, //height
		NULL, //handle to parent
		NULL, //handle to menu
		m_hinstance, //application instance
		this); //pointer to the openglwindow

	//Check if window creation failed
	if(!m_hwnd)
		return 0;

	m_hdc = GetDC(m_hwnd); //Set the device context

	ShowWindow(m_hwnd, SW_SHOW); //Display the window
	UpdateWindow(m_hwnd); //Update window

	return true;
}

void MSOpenGLWindow::destroy()
{
	if(m_isFullscreen)
	{
		ChangeDisplaySettings(NULL, 0); //Switch back to desktop
		ShowCursor(true);
	}
}

void MSOpenGLWindow::setupPixelFormat(void)
{
    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),  // size
            1,                          // version
            PFD_SUPPORT_OPENGL |        // OpenGL window
            PFD_DRAW_TO_WINDOW |        // render to window
            PFD_DOUBLEBUFFER,           // support double-buffering
            PFD_TYPE_RGBA,              // color type
            32,                         // prefered color depth
            0, 0, 0, 0, 0, 0,           // color bits (ignored)
            0,                          // no alpha buffer
            0,                          // alpha bits (ignored)
            0,                          // no accumulation buffer
            0, 0, 0, 0,                 // accum bits (ignored)
            16,                         // depth buffer
            0,                          // no stencil buffer
            0,                          // no auxiliary buffers
            PFD_MAIN_PLANE,             // main layer
            0,                          // reserved
            0, 0, 0,                    // no layer, visible, damage masks
    };

    pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    SetPixelFormat(m_hdc, pixelFormat, &pfd);
}

LRESULT CALLBACK MSOpenGLWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MSOpenGLWindow* window = NULL;

	//Window is being created
	if(msg == WM_CREATE)
	{
		//Get the window pointer
		window = (MSOpenGLWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;

		//Associate the window pointer with hwnd
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)window);
	}
	//Not a creation
	else
	{
		//Should already have the pointer
		window = (MSOpenGLWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);

		if(!window)
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	//Call the window's member WndProc
	return window->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT MSOpenGLWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			m_hdc = GetDC(hWnd);
			setupPixelFormat();

			//Set the version the OpenGL version. In this case 3.0
			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 0,
				0}; //Zero indicates end of array

			//Create a temporary context
			HGLRC tmpContext = wglCreateContext(m_hdc);
			//Make it current
			wglMakeCurrent(m_hdc, tmpContext);

			//Get the function pointer
			wglCreateContextAttribsARB = (GLEEPFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

			//If this is NULL then version of OpenGL is not supported
			if(!wglCreateContextAttribsARB)
			{
				MessageBox(NULL, L"OpenGL 3.0 is not supported, falling back to OpenGL 2.1", L"An error occurred", MB_ICONERROR | MB_OK);
				m_hglrc = tmpContext;
			}
			else
			{
				//Create an OpenGL 3.0 context
				m_hglrc = wglCreateContextAttribsARB(m_hdc, 0, attribs);
				//Delete the temporary context
				wglDeleteContext(tmpContext);
			}

			//Make the context current
			wglMakeCurrent(m_hdc, m_hglrc);

			m_isRunning = true;
		}
		break;
		case WM_DESTROY:
		case WM_CLOSE:
			wglMakeCurrent(m_hdc, NULL);
			wglDeleteContext(m_hglrc);
			m_isRunning = false;
			PostQuitMessage(0);
			return 0;
		break;
		case WM_SIZE: //resize
		{
			int height = HIWORD(lParam);
			int width = LOWORD(lParam);
			//Resize
		}
		break;
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE)
				DestroyWindow(m_hwnd);
		break;
		case WM_KEYUP:
		break;
		default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
