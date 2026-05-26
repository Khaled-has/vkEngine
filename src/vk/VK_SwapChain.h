#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

#include <iostream>
#include <vector>

#include "VK_Device.h"

#define INLINE_CON inline const

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

	INLINE_CON std::vector<VkImage>& getImages() { return m_pImages; }
	INLINE_CON std::vector<VkImageView>& getImageViews() { return m_pImageViews; }

	INLINE_CON VkFormat getColorImageFormat() { return m_pSurfaceFormat.format; }

private:
	VkSwapchainKHR m_pSwapchain = VK_NULL_HANDLE;

	std::vector<VkImage> m_pImages;
	std::vector<VkImageView> m_pImageViews;

	VkSurfaceFormatKHR m_pSurfaceFormat{};

	std::vector<VkImage> m_pDepthImages;
	std::vector<VkImageView> m_pDepthImageViews;

	void CreateSwapchain(const VK_SwapChainInfo mInfo);
};

#endif