#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H

#include <vector>
#include <span>

#include "VK_CmdBuf.h"
#include "VK_Texture.h"

struct VK_PassInfo
{
	const VK_Texture* pColorTextures;
	uint32_t mColorCount;
	const VkClearValue* pClearColorValues;

	const VK_Texture* pDepthTexture;
	VkClearValue mClearDepthValue;

	VkRect2D mRenderArea;

	bool mIsColor;
	bool mIsDepth;
};

class VK_RenderPass
{
public:
	VK_RenderPass() {}

	void Create(std::span<VK_PassInfo> mPassInfos);

	void Begin(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex);
	void End(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex);
private:
	std::vector<VK_PassInfo> m_pPasses;
};

void BeginDynamicRendering(const VkCommandBuffer& CmdBuf, const VkImageView* pColorView, uint32_t pColorCount, const VkImageView* pDepthView, const VkClearValue* pClearColor, const VkClearValue* pDepthValue, bool IsColorAttch, bool IsDepthTest, VkRect2D mRenderArea);

#endif