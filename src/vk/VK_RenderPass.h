#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H

#include <vector>

#include "VK_CmdBuf.h"
#include "VK_Texture.h"

struct VK_PassInfo
{
	const VK_Texture* pColorTextures;
	uint32_t mColorCount;
	const VkClearValue* pClearColorValues;

	const VK_Texture* pDepthTexture;
	VkClearValue mClearDepthValue;

	VkImageLayout mInitLayout;
	VkImageLayout mNewLayout;
	VkImageLayout mFinalLayout;

	VkRect2D mRenderArea;

	bool mIsColor;
	bool mIsDepth;
};

class VK_RenderPass
{
public:
	VK_RenderPass() {}
	VK_RenderPass(const VK_PassInfo* pPassInfos, uint32_t mPassCount) { Create(pPassInfos, mPassCount); }

	void Create(const VK_PassInfo* pPassInfos, uint32_t mPassCount);

	void Begin(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex);
	void End(const VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex);
private:
	const VK_PassInfo* pPassInfos;
	uint32_t m_pPassCount;

	std::vector<std::vector<VkImageView>> m_pImageViews;
};

#endif