#ifndef MSOPENGLWINDOW_H
#define MSOPENGLWINDOW_H

#include "openglwindow.h"

class MSOpenGLWindow : public OpenGLWindow
{
public:
	MSOpenGLWindow(HINSTANCE hInstance);
	virtual ~MSOpenGLWindow();

	bool create(int width, int height, int bpp, bool fullscreen, wchar_t* name);
	void destroy();

	void swapBuffers() { SwapBuffers(m_hdc); }

	InputInterface* getInput() const { return m_input; }

	static LRESULT CALLBACK StaticWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	wchar_t* m_name;
	bool m_isFullscreen;
	bool m_isRunning;

	HWND m_hwnd; //Window handle
	HGLRC m_hglrc; //Rendering context
	HDC m_hdc; //Device context
	RECT m_windowRect; //Window bounds
	HINSTANCE m_hinstance; //Application instance
	WNDCLASSEX m_windowClass;

	InputInterface* m_input;

	void setupPixelFormat(void);
};

#endif
