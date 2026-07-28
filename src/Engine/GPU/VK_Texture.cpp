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
		.unnormalizedCoordinates = VK_FALSE,
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

void VK_Texture::CreateImage(VkExtent3D mExtent, VkFormat mFormat, VkImageUsageFlags mUsage, uint32_t mLayerCount)
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
		.usage = mUsage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};

	VmaAllocationCreateInfo allocInfo = {
		.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
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
	//CreateView(m_pImage, mFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1u);
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

static bool HasStencilComponent(VkFormat Format)
{
	return ((Format == VK_FORMAT_D32_SFLOAT_S8_UINT)
		|| (Format == VK_FORMAT_D24_UNORM_S8_UINT));
}

void TextureLayoutTransition(VK_Texture& mTex, const VK_CmdBuf& mCmdBuf, VkImageLayout mNewLayout)
{
	VkImageLayout mOldLayout = mTex.getLayout();
	VkFormat Format = mTex.getFormat();

	mTex.setLayout(mNewLayout);

	VkImageMemoryBarrier Barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = 0,
		.dstAccessMask = 0,
		.oldLayout = mOldLayout,
		.newLayout = mNewLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mTex.getImage(),
		.subresourceRange = VkImageSubresourceRange {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = (uint32_t)mTex.getLayerCount()
		}
	};

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

	if (mNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
		(Format == VK_FORMAT_D16_UNORM) ||
		(Format == VK_FORMAT_X8_D24_UNORM_PACK32) ||
		(Format == VK_FORMAT_D32_SFLOAT) ||
		(Format == VK_FORMAT_S8_UINT) ||
		(Format == VK_FORMAT_D16_UNORM_S8_UINT) ||
		(Format == VK_FORMAT_D24_UNORM_S8_UINT))
	{
		Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(Format)) {
			Barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_GENERAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		mNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} /* Convert back from read-only to updateable */
	else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} /* Convert from updateable texture to shader read-only */
	else if (mOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert depth texture from undefined state to depth-stencil buffer */
	else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	} /* Wait for render pass to complete */
	else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = 0; // VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = 0;
		/*
				sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		///		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		*/
		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert back from read-only to color attachment */
	else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	} /* Convert from updateable texture to shader read-only */
	else if (mOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert back from read-only to depth attachment */
	else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	} /* Convert from updateable depth texture to shader read-only */
	else if (mOldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (mOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
		Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
	else {
		//LOG_ERROR("Unknown Barrier case");
		std::cout << "Unknown Barrier case\n";
		exit(1);
	}

	vkCmdPipelineBarrier(mCmdBuf.getHandle(), sourceStage, destinationStage,
		0, 0, NULL, 0, NULL, 1, &Barrier);
}
