#include "VK_SwapChain.h"

#include "config.h"

#include <volk.h>

uint32_t ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities)
{
	uint32_t RequestedNumImage = Capabilities.minImageCount + 1;

	uint32_t FinalNumImages = 0;

	if ((Capabilities.maxImageCount > 0) && (RequestedNumImage > Capabilities.maxImageCount))
	{
		FinalNumImages = Capabilities.maxImageCount;
	}
	else {
		FinalNumImages = RequestedNumImage;
	}

	if (FinalNumImages > 3)
		FinalNumImages = 3;

	return FinalNumImages;
}

VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> PresentModes)
{
	for (uint32_t i = 0; i < PresentModes.size(); i++)
	{
		if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			LOG_INFO("Selected Present Mode Is: VK_PRESENT_MODE_MALIBOX_KHR");
			return PresentModes[i];
		}
	}
	LOG_INFO("Selected Present Mode Is: VK_PRESENT_MODE_FIFO_KHR");
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats)
{
	for (uint32_t i = 0; i < SurfaceFormats.size(); i++)
	{
		if ((SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM) &&
			(SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
		{
			return SurfaceFormats[i];
		}
	}
	LOG_WARN("No Format & Color Space");
	return SurfaceFormats[0];
}

void VK_SwapChain::Create(const VK_SwapChainInfo mInfo)
{
	CreateSwapchain(mInfo);
}

void VK_SwapChain::Destroy()
{
	const VkDevice& Device = VK_Device::Get()->getDevice();

	// # Destroy image views
	for (auto& Im : m_pImageViews)
	{
		vkDestroyImageView(Device, Im, NULL);
	}

	m_pImages.clear();
	m_pImageViews.clear();

	// # Destroy swapchain
	vkDestroySwapchainKHR(Device, m_pSwapchain, NULL);
}

void VK_SwapChain::CreateSwapchain(const VK_SwapChainInfo mInfo)
{
	VK_Device* Device = VK_Device::Get();

	// # Choose num swapchain images
	VkSurfaceCapabilitiesKHR SurfaceCaps;
	CHECK_VK_RES(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device->getSelectedPhysDevice().m_pPhysDev, Device->getSurface(), &SurfaceCaps), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

	uint32_t NumImages = ChooseNumImages(SurfaceCaps);

	// # Choose present mode
	const std::vector<VkPresentModeKHR> PresentModes = Device->getSelectedPhysDevice().m_presentModes;
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	if (mInfo.mEnableVSync)
	{
		PresentMode = ChoosePresentMode(PresentModes);
	}

	// # Choose swapchain surface format & color space
	m_pSurfaceFormat = ChooseSurfaceFormatAndColorSpace(Device->getSelectedPhysDevice().m_surfaceFormats);

	// # Create the swapchain
	VkSwapchainCreateInfoKHR SwapChainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = Device->getSurface(),
		.minImageCount = NumImages,
		.imageFormat = m_pSurfaceFormat.format,
		.imageColorSpace = m_pSurfaceFormat.colorSpace,
		.imageExtent = SurfaceCaps.currentExtent,
		.imageArrayLayers = 1,
		.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = SurfaceCaps.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = PresentMode,
		.clipped = VK_TRUE
	};
	
	CHECK_VK_RES(vkCreateSwapchainKHR(Device->getDevice(), &SwapChainCreateInfo, NULL, &m_pSwapchain), "vkCreateSwapchainKHR");

	// # Get the swapchain's images
	uint32_t NumSwapChainImages = 0;
	CHECK_VK_RES(vkGetSwapchainImagesKHR(Device->getDevice(), m_pSwapchain, &NumSwapChainImages, NULL), "vkGetSwapchainImagesKHR");
	NumImages = NumSwapChainImages;
	//assert(NumImages == NumSwapChainImages);

	m_pImages.resize(NumSwapChainImages);
	m_pImageViews.resize(NumSwapChainImages);

	CHECK_VK_RES(vkGetSwapchainImagesKHR(Device->getDevice(), m_pSwapchain, &NumSwapChainImages, m_pImages.data()), "vkGetSwapchainImagesKHR");

	// # Create swapchain image views
	for (uint32_t i = 0; i < m_pImageViews.size(); i++)
	{
		m_pImageViews[i] = CreateImageView(
			m_pImages[i], Device->getDevice(), m_pSurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT
		);
	}
}
