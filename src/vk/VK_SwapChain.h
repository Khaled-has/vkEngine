#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

#include <iostream>
#include <vector>

#include "VK_Texture.h"

struct VK_SwapChainInfo 
{
	bool mEnableDepth = false;
	bool mEnableVSync = true;
};

class VK_SwapChain
{
public:
	VK_SwapChain() {}

	void Create(const VK_SwapChainInfo mInfo);
	void Destroy();

	inline uint32_t getSwapChainImageCount() const { return m_pSwapImages; }

	inline const VkSwapchainKHR& getSwapchain() const { return m_pSwapchain; }

	inline const std::vector<VkImage>& getImages() const { return m_pImages; }

	inline const std::vector<VK_Texture>& getSwapChainTextures() const { return m_pTextures; }

	inline VkFormat getColorImageFormat() { return m_pSurfaceFormat.format; }

private:
	VkSwapchainKHR m_pSwapchain = VK_NULL_HANDLE;

	std::vector<VkImage> m_pImages;
	std::vector<VK_Texture> m_pTextures;

	VkSurfaceFormatKHR m_pSurfaceFormat{};

	uint32_t m_pSwapImages = 0;

	void CreateSwapchain(const VK_SwapChainInfo mInfo);
};

#endif