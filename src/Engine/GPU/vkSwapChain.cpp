#include "vkSwapChain.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{
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
				return PresentModes[i];
			}
		}
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

	void vkSwapChain::Create(bool mVsync)
	{
		CheckContext("Create vkSwapChain");

		if (GetCurrentContext()->getSwapChainRes().mInitialized)
		{
			GPU_LOG_WARN("vkGPU: You can't create more than one vkSwapChain in the context");
			return;
		}

		if (!GetCurrentContext()->getDeviceRes().mInitialized)
		{
			GPU_LOG_ERROR("vkGPU: Create the vkDevice Before you trying to create the vkSwapChain");
			exit(1);
		}

		const vkDevice* device = GetCurrentContext()->getDeviceRes().pDevice.get();
		const VkSurfaceKHR surface = GetCurrentContext()->getSurfaceRes().mSurface;

		// # Choose num swapchain images
		VkSurfaceCapabilitiesKHR SurfaceCaps;
		GPU_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->getPhysDevice().getHandle(), surface, &SurfaceCaps), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
		m_Extent = VkRect2D{ .offset {.x = 0, .y = 0}, .extent = SurfaceCaps.currentExtent };
		m_ImageCount = ChooseNumImages(SurfaceCaps);

		// # Choose present mode
		const std::vector<VkPresentModeKHR> PresentModes = device->getPhysDevice().getInfo().mPresentModes;
		VkPresentModeKHR PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		if (mVsync)
		{
			PresentMode = ChoosePresentMode(PresentModes);
		}

		// # Choose swapchain surface format & color space
		m_SurfaceFormat = ChooseSurfaceFormatAndColorSpace(device->getPhysDevice().getInfo().mSurfaceFormats);

		// # Create the swapchain
		VkSwapchainCreateInfoKHR SwapChainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = NULL,
			.flags = 0,
			.surface = surface,
			.minImageCount = m_ImageCount,
			.imageFormat = m_SurfaceFormat.format,
			.imageColorSpace = m_SurfaceFormat.colorSpace,
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

		GPU_CHECK(vkCreateSwapchainKHR(device->getHandle(), &SwapChainCreateInfo, NULL, &m_Swapchain), "vkCreateSwapchainKHR");

		// # Get the swapchain's images
		uint32_t NumSwapChainImages = 0;
		GPU_CHECK(vkGetSwapchainImagesKHR(device->getHandle(), m_Swapchain, &NumSwapChainImages, NULL), "vkGetSwapchainImagesKHR");
		m_ImageCount = NumSwapChainImages;
		GPU_ASSERT(m_ImageCount == NumSwapChainImages, "Num swapchain images != NumSwapChainImages");

		m_SwapTextures.resize(NumSwapChainImages);

		std::vector<VkImage> Images(NumSwapChainImages);
		m_SwapTextures.resize(NumSwapChainImages);

		GPU_CHECK(vkGetSwapchainImagesKHR(device->getHandle(), m_Swapchain, &NumSwapChainImages, Images.data()), "vkGetSwapchainImagesKHR");

		// # Create swapchain image views
		for (uint32_t i = 0; i < NumSwapChainImages; i++)
		{
			vkTextureInfo info = {
				.mFormat = m_SurfaceFormat.format,
				.mViewType = VK_IMAGE_VIEW_TYPE_2D,
				.mAspect = VK_IMAGE_ASPECT_COLOR_BIT,
				.mLayerCount = 1u
			};

			vkTextureSamplerInfo samplerInfo = {
				.mMinFilter = VK_FILTER_LINEAR,
				.mMaxFilter = VK_FILTER_LINEAR,
				.mAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
			};

			m_SwapTextures[i].CreateViewOnly(
				Images[i], info, samplerInfo
			);
		}
	}
	void vkSwapChain::Destroy()
	{
	}
}