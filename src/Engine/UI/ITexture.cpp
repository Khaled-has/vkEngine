#include "ITexture.h"

#include <imgui_impl_vulkan.h>
#include <stb_image.h>

namespace UI
{
	void ITexture::load(const char* pFileName)
	{
		int x = 0; int y = 0; int channels = 0;
		unsigned char* pData = stbi_load(pFileName, &x, &y, &channels, STBI_rgb_alpha);

		mTexInfo =
		{
			.mWidth = (uint32_t)x,
			.mHeight = (uint32_t)y,
			.mChannels = (uint32_t)channels
		};

		// # Create the image buffer data stage
		VkDeviceSize bufferSize = ((x * y) * channels) * sizeof(unsigned char);

		VkBufferCreateInfo BufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = bufferSize,
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		};

		VmaAllocationCreateInfo stageAllocInfo = {
			.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		VK_Buffer stageBuffer;
		stageBuffer.Create(BufferInfo, stageAllocInfo);

		memcpy(stageBuffer.getAllocInfo().pMappedData, pData, bufferSize);

		mTexture.CreateImage(
			{ .width = mTexInfo.mWidth, .height = mTexInfo.mHeight, .depth = 1 },
			VK_FORMAT_B8G8R8_UNORM,
			1
		);
		mTexture.CreateView(
			mTexture.getImage(), mTexture.getFormat(),
			VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT
		);
		mTexture.CreateSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);

		VkBufferImageCopy region[] = {
			{
				.bufferOffset = 0,
				.imageOffset = 0,
				.imageExtent = mTexture.getExtent3D()
			}
		};

		CopyBufferToImage(stageBuffer, mTexture, region);

		mDescSet = ImGui_ImplVulkan_AddTexture(
			mTexture.getSampler(), mTexture.getView(), mTexture.getLayout()
		);

		mID = (uint32_t)mDescSet;
	}
}