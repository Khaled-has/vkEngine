#include "VK_RenderPass.h"

#include "config.h"

bool HasStencilComponent(VkFormat Format)
{
	return ((Format == VK_FORMAT_D32_SFLOAT_S8_UINT) 
		 || (Format == VK_FORMAT_D24_UNORM_S8_UINT));
}

void ImageMemBarrier(const VkCommandBuffer& CmdBuf, VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount)
{
	VkImageMemoryBarrier Barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = 0,
		.dstAccessMask = 0,
		.oldLayout = OldLayout,
		.newLayout = NewLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = Image,
		.subresourceRange = VkImageSubresourceRange {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = (uint32_t)LayerCount
		}
	};

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

	if (NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
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

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_GENERAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} /* Convert back from read-only to updateable */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} /* Convert from updateable texture to shader read-only */
	else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert depth texture from undefined state to depth-stencil buffer */
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	} /* Wait for render pass to complete */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
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
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	} /* Convert from updateable texture to shader read-only */
	else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} /* Convert back from read-only to depth attachment */
	else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	} /* Convert from updateable depth texture to shader read-only */
	else if (OldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		Barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
		Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		Barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
	else {
		LOG_ERROR("Unknown Barrier case");
		exit(1);
	}

	vkCmdPipelineBarrier(CmdBuf, sourceStage, destinationStage,
		0, 0, NULL, 0, NULL, 1, &Barrier);
}

void BeginDynamicRendering(const VkCommandBuffer& CmdBuf, const VkImageView* pColorView, uint32_t pColorCount, const VkImageView* pDepthView, const VkClearValue* pClearColor, const VkClearValue* pDepthValue, bool IsColorAttch, bool IsDepthTest, VkRect2D mRenderArea)
{

	std::vector <VkRenderingAttachmentInfoKHR> ColorAttachments;
	for (uint32_t i = 0; i < pColorCount; i++)
	{
		VkRenderingAttachmentInfoKHR ColorAttach = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.pNext = NULL,
			.imageView = *pColorView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = pClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE
		};

		if (IsColorAttch)
		{
			ColorAttach.clearValue = pClearColor[i];
		}

		ColorAttachments.push_back(ColorAttach);
	}

	VkRenderingAttachmentInfo DepthAttachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = NULL,
		.imageView = IsDepthTest ? *pDepthView : VK_NULL_HANDLE,
		.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.resolveImageView = VK_NULL_HANDLE,
		.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.loadOp = pDepthValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE
	};

	if (IsDepthTest)
	{
		DepthAttachment.clearValue = *pDepthValue;
	}

	VkRenderingInfoKHR RenderingInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.renderArea = mRenderArea,
		.layerCount = 1,
		.viewMask = 0,
		.colorAttachmentCount = pColorCount,
		.pColorAttachments = ColorAttachments.data(),
		.pDepthAttachment = IsDepthTest ? &DepthAttachment : VK_NULL_HANDLE
	};

	vkCmdBeginRendering(CmdBuf, &RenderingInfo);
}

void VK_RenderPass::Create(const VK_PassInfo* pPassInfos, uint32_t mPassCount)
{
	this->pPassInfos = pPassInfos;
	m_pPassCount = mPassCount;
	
	m_pImageViews.resize(mPassCount);
	for (uint32_t i = 0; i < mPassCount; i++)
	{
		for (uint32_t j = 0; j < pPassInfos[i].mColorCount; j++)
		{
			m_pImageViews[i].push_back(pPassInfos[i].pColorTextures[j].getView());
		}
	}
}

void VK_RenderPass::Begin(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex)
{
	std::vector<VkImageView> ColorViews;

	// # Image colors barrier
	for (uint32_t i = 0; i < pPassInfos[mFrameIndex].mColorCount; i++)
	{
		const VK_Texture* CurTex = &pPassInfos[mFrameIndex].pColorTextures[i];
		
		ColorViews.push_back(CurTex->getView());

		ImageMemBarrier(
			mCmdBuf.getHandle(), CurTex->getImage(),
			CurTex->getFormat(), pPassInfos[mFrameIndex].mInitLayout, pPassInfos[mFrameIndex].mNewLayout,
			1
		);
	}

	// # Dynamic rendering
	BeginDynamicRendering(
		mCmdBuf.getHandle(),
		ColorViews.data(), (uint32_t)ColorViews.size(),
		&pPassInfos[mFrameIndex].pDepthTexture->getView(),
		pPassInfos[mFrameIndex].pClearColorValues, &pPassInfos[mFrameIndex].mClearDepthValue,
		pPassInfos[mFrameIndex].mIsColor, pPassInfos[mFrameIndex].mIsDepth,
		pPassInfos[mFrameIndex].mRenderArea
	);
}

void VK_RenderPass::End(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex)
{
	vkCmdEndRendering(mCmdBuf.getHandle());

	// # Image colors barrier
	for (uint32_t i = 0; i < pPassInfos[mFrameIndex].mColorCount; i++)
	{
		const VK_Texture* CurTex = &pPassInfos[mFrameIndex].pColorTextures[i];

		ImageMemBarrier(
			mCmdBuf.getHandle(), CurTex->getImage(),
			CurTex->getFormat(), pPassInfos[mFrameIndex].mNewLayout, pPassInfos[mFrameIndex].mFinalLayout,
			1
		);
	}
}
