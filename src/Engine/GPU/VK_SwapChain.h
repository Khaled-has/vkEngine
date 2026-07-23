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

	inline std::vector<VK_Texture>& getSwapChainTextures() { return m_pTextures; }

	inline VkFormat getColorImageFormat() const { return m_pSurfaceFormat.format; }
	inline VkRect2D getScreenRect() const { return m_pScreenRect; }

private:
	VkSwapchainKHR m_pSwapchain = VK_NULL_HANDLE;

	std::vector<VkImage> m_pImages;
	std::vector<VK_Texture> m_pTextures;

	VkRect2D m_pScreenRect;
	VkSurfaceFormatKHR m_pSurfaceFormat{};

	uint32_t m_pSwapImages = 0;

	void CreateSwapchain(const VK_SwapChainInfo mInfo);
};

#endif