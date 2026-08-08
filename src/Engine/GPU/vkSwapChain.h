#ifndef VKSWAPCHAIN_H
#define VKSWAPCHAIN_H

#include "vkTexture.h"

namespace GPU
{
	class VKGPU_API vkSwapChain
	{
	public:
		vkSwapChain() {}

		inline const VkSwapchainKHR& getHandle()					const { return m_Swapchain; }

		inline uint32_t getSwapImageCount()						const { return m_ImageCount; }
		inline VkRect2D getSwapExtent()							const { return m_Extent; }

		inline const std::vector<vkTexture>& getSwapTextures()	const { return m_SwapTextures; }

	private:
		friend vkSwapChain* CreateSwapChain(bool mVSync);

		void Create(bool mVSync);
		void Destroy();

	private:
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		std::vector<vkTexture> m_SwapTextures;

		VkRect2D m_Extent;
		uint32_t m_ImageCount = 0;
		VkSurfaceFormatKHR m_SurfaceFormat;
	};
}

#endif