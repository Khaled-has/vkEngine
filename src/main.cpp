#include "vk/VK_Device.h"
#include "vk/VK_SwapChain.h"
#include "vk/log.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

int main(int argc, char* argv[])
{
	Log log;
	log.Init();
	LOG_TRACE("Hello vkEngine");

	static SDL_Window* pAppWin = SDL_CreateWindow(
		"vkEngine", 1440, 720, SDL_WINDOW_VULKAN
	);

	VK_Device mDevice;
	mDevice.Initialize([](VkSurfaceKHR* pSurface, VkInstance pInstance){
		if (!SDL_Vulkan_CreateSurface(pAppWin, pInstance, NULL, pSurface))
		{
			LOG_ERROR("Vulkan Error: SDL_VulkanCreateSurface");
		}
	});

	//VK_SwapChain mSwapChain;
	//mSwapChain.Create({.mEnableDepth = false, .mEnableVSync = true});

	bool pRunning = true;
	SDL_Event ev;
	while (pRunning)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
				pRunning = false;
		}

	}


	return 0;
}