#include "VK_Texture.h"

#include "config.h"
#include "VK_Device.h"

VK_TextureView::VK_TextureView(const VkImage& mImage, VkFormat mFormat, VkImageViewType mViewType, VkImageAspectFlags mAspectFlags, uint32_t mLayerCount)
{
	CreateView(mImage, mFormat, mViewType, mAspectFlags, mLayerCount);
}

void VK_TextureView::CreateView(const VkImage& mImage, VkFormat mFormat, VkImageViewType mViewType, VkImageAspectFlags mAspectFlags, uint32_t mLayerCount)
{
	if (m_pIsCreated)
	{
		LOG_WARN("This texture view is created before");
		return;
	}

	VkImageViewCreateInfo ViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = mImage,
		.viewType = mViewType,
		.format = mFormat,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY,
		},
		.subresourceRange = {
			.aspectMask = mAspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = mLayerCount
		}
	};

	CHECK_VK_RES(vkCreateImageView(VK_Device::Get()->getDevice(), &ViewInfo, NULL, &m_pView), "vkCreateImageView");
	m_pIsCreated = true;
}

void VK_TextureView::DestroyView()
{
	if (!m_pIsCreated)
	{
		LOG_WARN("the texture view not created yet: create it before you destroy it");
	}

	vkDestroyImageView(VK_Device::Get()->getDevice(), m_pView, NULL);
	m_pView = VK_NULL_HANDLE;

	m_pIsCreated = false;
}

bool VK_Texture::CreateImage(const void* pData, VkExtent2D mExtent2D, VkFormat mForamt, uint32_t mLayerCount)
{
	return true;
}

void VK_Texture::DestroyImage()
{
	vmaDestroyImage(VK_Device::Get()->getAllocator(), m_pImage, m_pAllocation);
}
