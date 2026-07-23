#include "VK_RenderPass.h"

#include "config.h"

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

		if (IsColorAttch && pClearColor)
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

	if (IsDepthTest && pDepthValue)
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

void VK_RenderPass::Create(std::span<VK_PassInfo> mPassInfos)
{	
	for (auto& In : mPassInfos)
	{
		m_pPasses.push_back(In);
	}
}

void VK_RenderPass::Begin(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex)
{
	std::vector<VkImageView> ColorViews;

	// # Image colors barrier
	for (uint32_t i = 0; i < m_pPasses[mFrameIndex].mColorCount; i++)
	{
		ColorViews.push_back(m_pPasses[mFrameIndex].pColorTextures[i].getView());
	}
	
	// # Dynamic rendering
	BeginDynamicRendering(
		mCmdBuf.getHandle(),
		ColorViews.data(), (uint32_t)ColorViews.size(),
		&m_pPasses[mFrameIndex].pDepthTexture->getView(),
		m_pPasses[mFrameIndex].pClearColorValues, &m_pPasses[mFrameIndex].mClearDepthValue,
		m_pPasses[mFrameIndex].mIsColor, m_pPasses[mFrameIndex].mIsDepth,
		m_pPasses[mFrameIndex].mRenderArea
	);
}

void VK_RenderPass::End(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex)
{
	vkCmdEndRendering(mCmdBuf.getHandle());
}
