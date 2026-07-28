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
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS
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

			Window::m_Info.mWidth = (uint32_t)width;
			Window::m_Info.mHeight = (uint32_t)height;
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

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(Window::m_Info.mWidth, Window::m_Info.mHeight);

	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		viewport->Flags |= ImGuiViewportFlags_NoDecoration;
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + 55.0f));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - 55.0f));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGui::PopStyleVar();
	// Submit the DockSpace
	// REMINDER: THIS IS A DEMO FOR ADVANCED USAGE OF DockSpace()!
	// MOST REGULAR APPLICATIONS WILL SIMPLY WANT TO CALL DockSpaceOverViewport(). READ COMMENTS ABOVE.
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}

void WindowsWindow::Maximize()
{
	m_CustumTBar.MaximizeWindow();
}

void WindowsWindow::Minimize()
{
	m_CustumTBar.MinimizeWindow();
}
