#include "renderer/renderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

int main(int argc, char* argv[])
{
	Log log;
	log.Init();
	LOG_TRACE("Hello vkEngine");

	SDL_Window* pAppWin = SDL_CreateWindow(
		"vkEngine", 1440, 720, SDL_WINDOW_VULKAN
	);

	renderer mRenderer;
	mRenderer.Initialize(pAppWin);

	bool pRunning = true;
	SDL_Event ev;
	while (pRunning)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
				pRunning = false;
		}

		mRenderer.render();
	}

	SDL_DestroyWindow(pAppWin);
	SDL_Quit();

	return 0;
}