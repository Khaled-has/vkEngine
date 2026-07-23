#pragma once

#include "vkGPU.h"

class ImGuiRenderer
{
public:
	ImGuiRenderer() {}

	void Initialize(const VK_Device& mDevice, VK_SwapChain& mSwapChain, const VK_CmdPool& mCmdPool);

	const VK_CmdBuf& getCmdBuf(
		uint32_t mFrameIndex, 
		std::function<void(const VkCommandBuffer&, VkImage, VkFormat, VkImageLayout, VkImageLayout, int)> FuncImageMemBarrier
	);

	void Render();

	void Destroy();

private:
	std::vector<VK_CmdBuf> mCmdBufs;

	std::vector<VkImage> mImages;
	std::vector<VkImageView> mViews;
	VkFormat mFormat;
	VkRect2D mRenderArea;

	VkDescriptorPool mDescPool = VK_NULL_HANDLE;

	void ImGuiStyleInit();
};