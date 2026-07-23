#include "RenderGraph.h"

static bool HasStencilComponent(VkFormat Format)
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
		//LOG_ERROR("Unknown Barrier case");
		std::cout << "Unknown Barrier case\n";
		exit(1);
	}

	vkCmdPipelineBarrier(CmdBuf, sourceStage, destinationStage,
		0, 0, NULL, 0, NULL, 1, &Barrier);
}

void RenderGraph::AddPass(
	const char* mName, 
	std::function<void(Pass&)> mSetup, std::function<void(PassInitialization&)> mInit,
	std::function<void(PassExecution&)> mRun
)
{
	mPassLambdas.push_back(
		PassInfo{
			.mPass = std::make_unique<Pass>(
				std::bind(&RenderGraph::getImageIndex, this, std::placeholders::_1),
				std::bind(&RenderGraph::addColorImage, this, std::placeholders::_1, std::placeholders::_2)
			),
			.mName = mName,
			.mSetup = mSetup,
			.mInit = mInit,
			.mRun = mRun
		}
	);
}

void RenderGraph::Setup()
{
	for (uint32_t i = 0; i < mPassLambdas.size(); i++)
	{
		std::cout << "Setup pass: " << mPassLambdas[i].mName << std::endl;
		mPassLambdas[i].mSetup(*mPassLambdas[i].mPass);
	}

	for (uint32_t i = 0; i < mPassLambdas.size(); i++)
	{
		std::cout << "Initialize pass: " << mPassLambdas[i].mName << std::endl;

		mPassLambdas[i].mInit(mPassExecution);
	}
}

void RenderGraph::Destroy()
{
	for (auto& Im : mImages)
	{
		for (auto& Tex : Im.mImages)
		{
			Tex.DestroySampler();
			Tex.DestroyView();
			Tex.DestroyImage();
		}
	}
}

void RenderGraph::Run(VK_CmdBuf& mCmdBufs, uint32_t mIndex)
{
	for (uint32_t j = 0; j < mPassLambdas.size(); j++)
	{
		for (uint32_t b = 0; b < mPassLambdas[j].mPass->mWritesImageIndices.size(); b++)
		{
			VK_Texture& mTex = mImages[mPassLambdas[j].mPass->mWritesImageIndices[b]].mImages[mIndex];
			// Writes barriers
			ImageMemBarrier(
				mCmdBufs.getHandle(),
				mTex.getImage(),
				mTex.getFormat(),
				mTex.getLayout(),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				1
			);

			mTex.setLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			std::cout << "Writes Image Name: " << mImages[mPassLambdas[j].mPass->mWritesImageIndices[b]].mName << " | Frame index: " << mIndex << std::endl;
		}
		PassExecution execution = {
			.mCmdBuf = mCmdBufs,
			.mFrameIndex = mIndex
		};

		mPassLambdas[j].mRun(execution);
	}
}

VK_Texture& RenderGraph::PrepareToRead(const char* mName, VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex)
{
	VK_Texture& mTex = getImage(mName).mImages[mFrameIndex];
	// Writes barriers
	ImageMemBarrier(
		mCmdBuf.getHandle(),
		mTex.getImage(),
		mTex.getFormat(),
		mTex.getLayout(),
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		1
	);

	mTex.setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	return mTex;
}

uint32_t RenderGraph::getImageIndex(std::string mName)
{
	for (uint32_t i = 0; i < mImages.size(); i++)
	{
		if (mName == mImages[i].mName)
		{
			return i;
		}
	}

	std::cout << "RenderGraph error: " << mName << "don't found on the attachments\n";
	return 0;
}

Image& RenderGraph::getImage(std::string mName)
{
	for (uint32_t i = 0; i < mImages.size(); i++)
	{
		if (mName == mImages[i].mName)
		{
			return mImages[i];
		}
	}

	std::cout << "RenderGraph: Initialize error would'nt found ( Image: " << mName << " )\n";
	exit(1);
}

void RenderGraph::addColorImage(std::string mName, VkFormat mFormat)
{
	mImages.push_back(Image());

	// TODO HERE ( CHANGE THE COUNT OF IMAGES TO THE SWAPCHAIN'S IMAGES COUNT )
	mImages.back().mImages.resize(3);
	mImages.back().mName = mName;
	for (auto& Im : mImages.back().mImages)
	{
		Im.CreateImage(VkExtent3D{ .width = 1440, .height = 720, .depth = 1 }, mFormat, 1);
		Im.CreateSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);
	}
}

// # Pass structure

void Pass::WritesImage(const char* mName)
{
	mWritesImageIndices.push_back(func_getImageIndex(mName));
}

void Pass::AddColorAttachment(const char* mName, VkFormat mFormat)
{
	func_addColorImage(mName, mFormat);
}

Image& PassInitialization::getImage(std::string mName)
{
	return func_getImage(mName);
}
