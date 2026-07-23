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

	this->mFormat = mFormat;
	this->mLayerCount = mLayerCount;

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

void VK_TextureView::CreateSampler(VkFilter mMinFilter, VkFilter mMaxFilter, VkSamplerAddressMode mAddressMode)
{
	const VkDevice& pDevice = VK_Device::Get()->getDevice();

	VkSamplerCreateInfo SamplerInfo = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = mMinFilter,
		.minFilter = mMaxFilter,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = mAddressMode,
		.addressModeV = mAddressMode,
		.addressModeW = mAddressMode,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};

	CHECK_VK_RES(vkCreateSampler(pDevice, &SamplerInfo, VK_NULL_HANDLE, &m_pSampler), "vkCreateSampler");
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

void VK_TextureView::DestroySampler()
{
	vkDestroySampler(VK_Device::Get()->getDevice(), m_pSampler, NULL);
}

void VK_Texture::CreateImage(VkExtent3D mExtent, VkFormat mFormat, uint32_t mLayerCount)
{
	mExtent3D = mExtent;
	this->mFormat = mFormat;
	this->mLayerCount = mLayerCount;

	VkImageCreateInfo imageInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = mFormat,
		.extent = mExtent,
		.mipLevels = 1,
		.arrayLayers = mLayerCount,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};

	VmaAllocationCreateInfo allocInfo = {
		.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO
	};

	CHECK_VK_RES(
		vmaCreateImage(
		VK_Device::Get()->getAllocator(),
		&imageInfo, &allocInfo,
		&m_pImage, &m_pAllocation,
		nullptr
		), 
		"vmaCreateImage"
	);

	// # Create image view
	CreateView(m_pImage, mFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1u);
}

void VK_Texture::DestroyImage()
{
	vmaDestroyImage(VK_Device::Get()->getAllocator(), m_pImage, m_pAllocation);
}


void CopyBufferToImage(const VK_Buffer& mBuffer, VK_Texture& mImage, std::span<VkBufferImageCopy> pRegions)
{
	const VK_CmdBuf& cmdBuf = VK_Device::Get()->getCmdBuf();

	// # Begin command buffer
	BeginCmdBuf(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// # copy command
	vkCmdCopyBufferToImage(
		cmdBuf.getHandle(),
		mBuffer.getBuffer(),
		mImage.getImage(), mImage.getLayout(),
		(uint32_t)pRegions.size(), pRegions.data()
	);

	// # End command buffer
	vkEndCommandBuffer(cmdBuf.getHandle());

	// # Submit on transfer queue
	VK_Device::Get()->getQueues().mTransfer->SubmitSync(&cmdBuf.getHandle(), 1);
	vkQueueWaitIdle(VK_Device::Get()->getQueues().mTransfer->getHandle());
}