#ifdef _WIN32
#include <windows.h>
#include "msopenglwindow.h"
#else
#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
#else
int main(int argc, char** argv)
#endif
{
	//Set window settings
	const int windowWidth = 1024;
	const int windowHeight = 768;
	const int windowBPP = 16;
	bool windowFullscreen = false;

	//Declare window
#ifdef _WIN32
	MSOpenGLWindow programWindow(hInstance);
#else
#endif

	//Create window
	if(!programWindow.create(windowWidth, windowHeight, windowBPP, windowFullscreen, L"heya"))
	{
#ifdef _WIN32
		MessageBox(NULL, L"Unable to create the window", L"An error occurred", MB_ICONERROR | MB_OK);
#endif
		programWindow.destroy();
		return 1;
	}

	while(true)
	{
	}
	return 0;
}
