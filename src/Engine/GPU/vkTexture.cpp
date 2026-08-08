#include "vkTexture.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{
	void vkTexture::CreateViewOnly(VkImage mImage, vkTextureInfo mInfo, vkTextureSamplerInfo mSamplerInfo)
	{
		CheckContext("Create vkTexture");

		m_Info = mInfo;
		m_SamplerInfo = mSamplerInfo;

		if (CheckLoad()) { return; }

		// # Create image view
		VkImageViewCreateInfo ViewInfo =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.image = mImage,
			.viewType = mInfo.mViewType,
			.format = mInfo.mFormat,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			.subresourceRange = {
				.aspectMask = mInfo.mAspect,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = mInfo.mLayerCount
			}
		};

		GPU_CHECK(vkCreateImageView(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &ViewInfo, NULL, &m_View),
			"vkCreateImageView"
		);

		// # Create sampler
		CreateSampler(mSamplerInfo);

		m_Created = true;
	}

	void vkTexture::CreateSampler(vkTextureSamplerInfo mSamplerInfo)
	{
		VkSamplerCreateInfo SamplerInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = mSamplerInfo.mMinFilter,
			.minFilter = mSamplerInfo.mMaxFilter,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = mSamplerInfo.mAddressMode,
			.addressModeV = mSamplerInfo.mAddressMode,
			.addressModeW = mSamplerInfo.mAddressMode,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 1,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		};

		GPU_CHECK(vkCreateSampler(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &SamplerInfo, VK_NULL_HANDLE, &m_Sampler),
			"vkCreateSampler"
		);
	}

	bool vkTexture::CheckLoad()
	{
		if (m_Created)
		{
			GPU_LOG_WARN("vkGPU: You can't create a vkTexture twice");
			return true;
		}
		else
		{
			return false;
		}
	}
}