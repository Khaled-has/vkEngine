#include "Window.h"

#include "Windows/WindowsWindow.h"

Window* CreateWindowsWindow(WindowInfo mInfo)
{
	return new WindowsWindow(mInfo);
}
