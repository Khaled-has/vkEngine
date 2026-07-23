#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <functional>

#include "vkGPU.h"

struct Image
{
	std::vector<VK_Texture> mImages;
	std::string mName;
};

struct Pass
{
	friend class RenderGraph;

	Pass(
		std::function<uint32_t(std::string)> Func_getImageIndex, 
		std::function<void(std::string, VkFormat)> Func_addColorImage
	)
	{
		func_getImageIndex = Func_getImageIndex;
		func_addColorImage = Func_addColorImage;
	}

	void ReadsImage(const char* mName, VkImageLayout mLayout);
	void WritesImage(const char* mName);

	void AddColorAttachment(const char* mName, VkFormat mFormat);

	void CreatesBuffer(const char* mName, size_t mSize, VkBufferUsageFlags mUsages);

	void ReadsBuffer(const char* mName);
	void WritesBuffer(const char* mName);

	std::function<uint32_t(std::string)> func_getImageIndex;
	std::function<void(std::string, VkFormat)> func_addColorImage;

private:
	std::vector<uint32_t> mReadsImageIndices;
	std::vector<uint32_t> mWritesImageIndices;
};

struct PassInitialization
{
	PassInitialization(std::function<Image&(std::string)> Func_getImage, const VK_Device& device, const VK_CmdBuf& cmdBuf)
		: mDevice(device), mCmdBuf(cmdBuf)
	{
		func_getImage = Func_getImage;
	}

	Image& getImage(std::string mName);

	// Delete it letter
	const VK_Device& mDevice;
	const VK_CmdBuf& mCmdBuf;

private:
	std::function<Image&(std::string)> func_getImage;
};

struct PassExecution
{
	VK_CmdBuf& mCmdBuf;
	uint32_t mFrameIndex;
};

class RenderGraph
{
public:
	RenderGraph(const VK_Device& mDevice, const VK_CmdBuf& mCmdBuf) 
		: mPassExecution(std::bind(&RenderGraph::getImage, this, std::placeholders::_1), mDevice, mCmdBuf)
	{}

	void AddPass(
		const char* mName,
		std::function<void(Pass&)> mSetup, std::function<void(PassInitialization&)> mInit, 
		std::function<void(PassExecution&)> mRun
	);

	void Setup();
	void Destroy();

	void Run(VK_CmdBuf& mCmdBufs, uint32_t mIndex);

	Image& getImage(std::string mName);
	VK_Texture& PrepareToRead(const char* mName, VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex);

private:
	PassInitialization mPassExecution;

	struct PassInfo
	{
		std::unique_ptr<Pass> mPass;
		std::string mName;

		std::function<void(Pass&)> mSetup; 
		std::function<void(PassInitialization&)> mInit;
		std::function<void(PassExecution&)> mRun;
	};

	std::vector<PassInfo> mPassLambdas;

	std::vector<Image> mImages;

	uint32_t getImageIndex(std::string mName);
	void addColorImage(std::string mName, VkFormat mFormat);
};

void ImageMemBarrier(const VkCommandBuffer& CmdBuf, VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount);