#include "WindowsWindow.h"

#include <SDL3/SDL_vulkan.h>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

Window* Window::m_pInstance;

void WindowsWindow::Create()
{
	SDL_Init(SDL_INIT_VIDEO);

	m_pWindow = SDL_CreateWindow(
		m_Info.mTitle.c_str(),
		m_Info.mWidth, m_Info.mHeight,
		SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS
	);

	m_CustumTBar.AttachCustumTBar(m_pWindow);

	m_pInstance = this;
}

void WindowsWindow::Exit()
{
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

void WindowsWindow::ImGui_ImplWindow_Init()
{
	ImGui_ImplSDL3_InitForVulkan(m_pWindow);
}

bool WindowsWindow::CreateVkSurface(void* mInstance, void* pSurface)
{
	VkInstance instance = static_cast<VkInstance>(mInstance);
	VkSurfaceKHR* surface = static_cast<VkSurfaceKHR*>(pSurface);
	return SDL_Vulkan_CreateSurface(
		m_pWindow, 
		instance, NULL, 
		surface
	);
}

void WindowsWindow::OnUpdate()
{
	while (SDL_PollEvent(&m_event))
	{
		// # ImGui process event
		ImGui_ImplSDL3_ProcessEvent(&m_event);

		// # Custum t-bar process
		m_CustumTBar.WIN32CustomTBarProcess(&m_event);

		// # Application events
		Event* event;
		// # Close window
		if (m_event.type == SDL_EVENT_QUIT)
		{
			event = new WindowCloseEvent;

			m_EventCallback(event);
		}
		// # Resize window
		else if (m_event.type == SDL_EVENT_WINDOW_RESIZED)
		{
			int width, height;
			SDL_GetWindowSize(m_pWindow, &width, &height);
			event = new WindowResizeEvent((unsigned int)width, (unsigned int)height);

			m_EventCallback(event);
		}
		// # Keyboard events
		else if (m_event.type == SDL_EVENT_KEY_DOWN)
		{
			int keycode = m_event.key.key;
			int repeat = 0;
			event = new KeyPressedEvent(keycode, repeat);

			m_EventCallback(event);
		}
	}

	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}
