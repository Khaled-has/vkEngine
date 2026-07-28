#pragma once

#include <SDL3/SDL.h>

class WIN32_CustumTBar_ImplSDL3
{
public:
    WIN32_CustumTBar_ImplSDL3() {}

    void AttachCustumTBar(SDL_Window* pWindow);

    void WIN32CustomTBarProcess(SDL_Event* ev);

    void MinimizeWindow();
    void MaximizeWindow();

private:
    SDL_Window* m_pWindow;
};