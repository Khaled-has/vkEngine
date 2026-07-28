#pragma once

#include "Window.h"
#include "WIN32_CustumTBar_ImplSDL3.h"

#include <SDL3/SDL.h>

class WindowsWindow : public Window
{
public:
	WindowsWindow(WindowInfo mInfo) : Window(mInfo) {}

	virtual void Create() override;
	virtual void Exit() override;

	virtual void ImGui_ImplWindow_Init() override;
	virtual bool CreateVkSurface(void* mInstance, void* pSurface) override;

	virtual void OnUpdate() override;

	virtual void Maximize() override;
	virtual void Minimize() override;

private:
	SDL_Window* m_pWindow;
	SDL_Event m_event;

	WIN32_CustumTBar_ImplSDL3 m_CustumTBar;
};