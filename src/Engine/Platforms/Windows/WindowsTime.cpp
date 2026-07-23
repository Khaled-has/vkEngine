#include "WindowsTime.h"

#ifdef WIN32
#include <SDL3/SDL.h>

Timer* Timer::m_pInstance = new WindowsTime();

void WindowsTime::StartImpl()
{
    m_StartTime = (uint32_t)SDL_GetPerformanceCounter();
}

uint32_t WindowsTime::getApplicationTimeImpl()
{
    return ((uint32_t)SDL_GetPerformanceCounter() - m_StartTime) / (uint32_t)SDL_GetPerformanceFrequency();
}

double WindowsTime::getTimeImpl()
{
    return (double)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
}

#endif