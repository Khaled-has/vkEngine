#include "WIN32_CustumTBar_ImplSDL3.h"

#include "Log.h"

#ifdef WIN32

#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>

#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <dwmapi.h>
//#pragma comment(lib, "dwmapi.lib")

static WNDPROC g_OriginalWndProc = nullptr;

void WIN32_CustumTBar_ImplSDL3::WIN32CustomTBarProcess(SDL_Event* ev)
{
		// Window move
		if (ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN && (ev->motion.y < 40))
		{
			SDL_Window* focus = SDL_GetKeyboardFocus();

			if (focus == m_pWindow)
			{
				// Window move process
				SDL_PropertiesID props = SDL_GetWindowProperties(m_pWindow);
				HWND hwnd = (HWND)SDL_GetPointerProperty(
					props,
					SDL_PROP_WINDOW_WIN32_HWND_POINTER,
					NULL
				);

				ASSERT(hwnd, "Failed to take HWND WIN32 for SDL3 window");

				ReleaseCapture();
				SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			}
		}

		// Minimum window size
		if (ev->type == SDL_EVENT_WINDOW_RESIZED)
		{
			int w, h;
			SDL_GetWindowSize(m_pWindow, &w, &h);

			if (w < 1440)
			{
				SDL_SetWindowSize(m_pWindow, 1440, h);
			}
			if (h < 720)
			{
				SDL_SetWindowSize(m_pWindow, w, 720);
			}
			
		}

}

// WIN32 For ReSizeable The Window
static LRESULT CALLBACK SDLWin32Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NCHITTEST:
	{
		POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ScreenToClient(hwnd, &ptMouse);

		RECT rc;
		GetClientRect(hwnd, &rc);

		const LONG border = 8;
		bool top = ptMouse.y < border;
		bool bottom = ptMouse.y >= rc.bottom - border;
		bool left = ptMouse.x < border;
		bool right = ptMouse.x >= rc.right - border;

		if (top && left) return HTTOPLEFT;
		if (top && right) return HTTOPRIGHT;
		if (bottom && left) return HTBOTTOMLEFT;
		if (bottom && right) return HTBOTTOMRIGHT;
		if (top) return HTTOP;
		if (bottom) return HTBOTTOM;
		if (left) return HTLEFT;
		if (right) return HTRIGHT;
	}
	}

	return CallWindowProcW((WNDPROC)g_OriginalWndProc, hwnd, msg, wParam, lParam);
}

void WIN32_CustumTBar_ImplSDL3::AttachCustumTBar(SDL_Window *pWindow)
{
    m_pWindow = pWindow;
    // Some window details ( WIN32 )
	SDL_PropertiesID props = SDL_GetWindowProperties(pWindow);

	HWND hwnd = (HWND)SDL_GetPointerProperty(
		props,
		SDL_PROP_WINDOW_WIN32_HWND_POINTER,
		NULL
	);

	ASSERT(hwnd, "Failed to take HWND WIN32 for SDL3 window");

	// Border Color ( Only Windows 11 )
	DWORD border = RGB(100, 100, 100);
	DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

	MARGINS margins = { 1, 1, 1, 1 };
	DwmExtendFrameIntoClientArea(hwnd, &margins);

	// Add Corner for the window ( Only Windows 11 )
	DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
	DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

	SetWindowPos(
		hwnd, NULL,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
		);

	// Set Window Native ReSizeable System
	g_OriginalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)SDLWin32Proc);

	// Update window's size for fixe some ui components
    int width = 0; 
    int height = 0;
    SDL_GetWindowSize(pWindow, &width, &height);
	SDL_SetWindowSize(pWindow,  - 40, height - 20);
	SDL_SetWindowSize(pWindow, width, height);
}

	void SetWin32Show(SDL_Window* win, int flag)
	{
		SDL_PropertiesID props = SDL_GetWindowProperties(win);
		HWND hwnd = (HWND)SDL_GetPointerProperty(
			props,
			SDL_PROP_WINDOW_WIN32_HWND_POINTER,
			NULL
		);

		ASSERT(hwnd, "Failed to takes HWND");
		ShowWindow(hwnd, flag);

		// TitleBar Color
		DWORD border = RGB(100, 100, 100);
		DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

		MARGINS margins = { 1, 1, 1, 1 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);

		DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
		DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

		// Update window's size for fixe some ui components
		SetWindowPos(
			hwnd, NULL,
			0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
		);

		int w, h;
		SDL_GetWindowSize(win, &w, &h);
		SDL_SetWindowSize(win, w - 20, h - 10);
		SDL_SetWindowSize(win, w, h);
	}

	// void WindowsWindow::CustomBar(bool* pRunning)
	// {
	// 	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.f, 15.f));

	// 	if (ImGui::BeginMainMenuBar())
	// 	{

		
	// 		ImGui::Spacing();
	// 		ImGui::Image((ImTextureRef)w_Prop.icon.GetID(), ImVec2(50, 50));
	// 		ImGui::Spacing();

	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
	// 		if (ImGui::BeginMenu("File"))
	// 		{
	// 			if (ImGui::MenuItem("FFF"))
	// 			{
	// 				ImGui::EndTabItem();
	// 			}


	// 			ImGui::EndMenu();
	// 		}
	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
	// 		if (ImGui::BeginMenu("Edit"))
	// 		{
	// 			if (ImGui::MenuItem("FFF"))
	// 			{
	// 				ImGui::EndTabItem();
	// 			}

	// 			ImGui::EndMenu();
	// 		}
	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
	// 		if (ImGui::BeginMenu("Window"))
	// 		{
	// 			if (ImGui::MenuItem("FFF"))
	// 			{
	// 				ImGui::EndTabItem();
	// 			}


	// 			ImGui::EndMenu();
	// 		}
	// 		// Title
	// 		ImGui::SameLine(((ImGui::GetWindowWidth() / 2) - 125.0f));
	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
	// 		ImGui::Text(w_Prop.Title.c_str());

	// 		/*ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);*/
	// 		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 250.0f);
	// 		if (ImGui::BeginMenu("Development"))
	// 		{
	// 			if (ImGui::MenuItem("FFF"))
	// 			{
	// 				ImGui::EndTabItem();
	// 			}


	// 			ImGui::EndMenu();
	// 		}

	// 		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
	// 		ImGui::SameLine(ImGui::GetWindowWidth() - 45.f);

	// 		if (ImGui::ImageButton("EXIE", (ImTextureRef)textureBar.GetID(), ImVec2(20.f, 20.f), ImVec2(0, 0), ImVec2(0.3333f, 1.0f)))
	// 		{
	// 			// Close The Application
	// 			*pRunning = false;
	// 		}

	// 		ImGui::SameLine(ImGui::GetWindowWidth() - 84.f);
	// 		if (ImGui::ImageButton("MAXMIZE", (ImTextureRef)textureBar.GetID(), ImVec2(20.f, 20.f), ImVec2(0.3333f, 0.0f), ImVec2(0.6666f, 1.0f)))
	// 		{
	// 			static bool max = false;
	// 			if (!max)
	// 			{
	// 				SetWin32Show(w_Window, SW_MAXIMIZE);
	// 				max = true;
	// 			}
	// 			else
	// 			{
	// 				SetWin32Show(w_Window, SW_SHOWDEFAULT);
	// 				max = false;
	// 			}
	// 		}

	// 		ImGui::SameLine(ImGui::GetWindowWidth() - 124.f);
	// 		if (ImGui::ImageButton("MIXMIZE", (ImTextureRef)textureBar.GetID(), ImVec2(20.f, 20.f), ImVec2(0.6666f, 0.0f), ImVec2(1.0f, 1.0f)))
	// 		{
	// 			SetWin32Show(w_Window, SW_MINIMIZE);
	// 		}

	// 	}
	// 	ImGui::EndMainMenuBar();
	// 	ImGui::PopStyleVar();

	// 	if (ImGui::Begin("Editor Style"))
	// 		ImGui::ShowStyleEditor();
	// 	ImGui::End();

	// }

    // TODO: class func
	// void ProcessChangesForWIN32()
	// {
	// 	SDL_PropertiesID props = SDL_GetWindowProperties(w_pWindow);

	// 	HWND hwnd = (HWND)SDL_GetPointerProperty(
	// 		props,
	// 		SDL_PROP_WINDOW_WIN32_HWND_POINTER,
	// 		NULL
	// 	);

	// 	ED_ASSERT(hwnd, "Failed to take HWND WIN32 for SDL3 window");

	// 	LONG style = GetWindowLong(hwnd, GWL_STYLE);

	// 	style &= ~(WS_CAPTION | WS_THICKFRAME);
	// 	style &= ~WS_SYSMENU;
	// 	style &= ~WS_MAXIMIZEBOX;
	// 	style &= ~WS_MINIMIZEBOX;

	// 	SetWindowLong(hwnd, GWL_STYLE, style);


	// 	// TitleBar Color
	// 	DWORD border = RGB(100, 100, 100);
	// 	DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

	// 	MARGINS margins = { 1, 1, 1, 1 };
	// 	DwmExtendFrameIntoClientArea(hwnd, &margins);

	// 	DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
	// 	DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

	// 	SetWindowPos(
	// 		hwnd, NULL,
	// 		0, 0, 0, 0,
	// 		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	// 	);

	// 	// Update window's size for fixe some ui components
	// 	SDL_SetWindowSize(w_Window, w_Prop.Width - 40, w_Prop.Height - 20);
	// 	SDL_SetWindowSize(w_Window, w_Prop.Width, w_Prop.Height);
	// }

#endif