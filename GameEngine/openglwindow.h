#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

// Each window should be keeping track of input differently so that windows out of focus don't get input
class InputInterface;

class OpenGLWindow
{
public:
	virtual ~OpenGLWindow() {}
	virtual bool create(int width, int height, int bpp, bool fullscreen, wchar_t* name) = 0;
	virtual void destroy() = 0;

	virtual InputInterface* getInput() const = 0;
};

#endif
