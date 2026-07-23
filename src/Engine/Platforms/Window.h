#pragma once

#include <iostream>
#include <cstdint>
#include <functional>

#include "Event.h"
#include "KeyEvent.h"
#include "ApplicationEvent.h"
#include "Code.h"

struct WindowInfo
{
	std::string mTitle;
	uint32_t mWidth;
	uint32_t mHeight;
};

class Window
{
public:
	Window(WindowInfo mInfo) : m_Info(mInfo) {}

	inline static Window* getInstance() { return m_pInstance; }

	virtual void Create() {}
	virtual void Exit() {}

	virtual void ImGui_ImplWindow_Init() = 0;
	virtual bool CreateVkSurface(void* mInstance, void* pSurface) = 0;

	virtual void OnUpdate() {}

	inline void SetEventCallback(std::function<void(Event*)> mEventCallback) { m_EventCallback = mEventCallback; }
	inline const WindowInfo& getWindowInfo() const { return m_Info; }

protected:
	static Window* m_pInstance;

	std::function<void(Event*)> m_EventCallback;
	WindowInfo m_Info;
};

Window* CreateWindowsWindow(WindowInfo mInfo);