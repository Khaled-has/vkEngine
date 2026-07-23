#pragma once

#include <iostream>
#include <functional>

#include "Window.h"
#include "Renderer.h"

#include "Event.h"
#include "KeyEvent.h"
#include "ApplicationEvent.h"

#include "LayerStack.h"

struct ApplicationInfo
{
	std::string mProjName;
	std::string mProjPath;
	std::function<void(void*)> mEditorInitFunc;
};

class Application
{
public:
	Application(ApplicationInfo mInfo);
	~Application();

private:
	Window* m_pAppWindow;
	Renderer* m_pRenderer;

	LayerStack* m_pLayerStack;

	bool m_AppRunning = true;

	void OnEvent(Event* event);

	bool OnKeyPressed(KeyPressedEvent& event);

	bool OnWindowResize(WindowResizeEvent& event);
	bool OnWindowClose(WindowCloseEvent& event);
};
