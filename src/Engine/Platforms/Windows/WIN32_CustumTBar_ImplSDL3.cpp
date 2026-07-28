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

	void WIN32_CustumTBar_ImplSDL3::MinimizeWindow()
	{
		SetWin32Show(m_pWindow, SW_MINIMIZE);
	}

	void WIN32_CustumTBar_ImplSDL3::MaximizeWindow()
	{
		static bool max = false;
		if (!max)
		{
			SetWin32Show(m_pWindow, SW_MAXIMIZE);
			max = true;
		}
		else
		{
			SetWin32Show(m_pWindow, SW_SHOWDEFAULT);
			max = false;
		}
	}

#endif