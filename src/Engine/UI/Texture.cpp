#include "Texture.h"
#include "UI.h"

#include <vkGPU.h>
#include <stb_image.h>
#include <imgui_impl_vulkan.h>

namespace UI
{

	void Texture::load(const char* pFileName)
	{
		int x = 0; int y = 0; int channels = 0;
		stbi_uc* pData = stbi_load(pFileName, &x, &y, &channels, STBI_rgb_alpha);

		mTexInfo =
		{
			.mWidth = (uint32_t)x,
			.mHeight = (uint32_t)y,
			.mChannels = (uint32_t)channels
		};

		// # Create the image buffer data stage
		VkDeviceSize bufferSize = x * y * 4;

		VkBufferCreateInfo BufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = bufferSize,
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};

		VmaAllocationCreateInfo stageAllocInfo = {
			.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		VK_Buffer stageBuffer;
		stageBuffer.Create(BufferInfo, stageAllocInfo);

		memcpy(stageBuffer.getAllocInfo().pMappedData, pData, static_cast<size_t>(bufferSize));
		stbi_image_free(pData);

		mTexture = new VK_Texture();
		VK_Texture* tex = static_cast<VK_Texture*>(mTexture);
		
		tex->CreateImage(
			{ .width = mTexInfo.mWidth, .height = mTexInfo.mHeight, .depth = 1 },
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
		);

		// # Create command buffer
		VK_CmdBuf cmd;
		VK_Device::Get()->getCmdPool().AllocCmdBufs(1, &cmd);

		BeginCmdBuf(cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		TextureLayoutTransition(*tex, cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy region[] = {
			{
				.bufferOffset = 0,
				.bufferRowLength = 0,
				.bufferImageHeight = 0,
				.imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1 },
				.imageOffset = {0, 0, 0},
				.imageExtent = tex->getExtent3D(),
			}
		};

		CopyBufferToImage(stageBuffer, *tex, region);
	
		TextureLayoutTransition(*tex, cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		
		vkEndCommandBuffer(cmd.getHandle());

		VK_Device::Get()->getQueues().mTransfer->SubmitSync(&cmd.getHandle(), 1);
		vkQueueWaitIdle(VK_Device::Get()->getQueues().mTransfer->getHandle());

		tex->CreateView(
			tex->getImage(), tex->getFormat(),
			VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT
		);
		tex->CreateSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);

		VkDescriptorSet mDescSet = ImGui_ImplVulkan_AddTexture(
			tex->getSampler(), tex->getView(), tex->getLayout()
		);

		mImID = (ImTextureID)mDescSet;
	}

	void UI::Texture::destroy()
	{
		reinterpret_cast<VK_Texture*>(mTexture)->DestroyView();
		reinterpret_cast<VK_Texture*>(mTexture)->DestroySampler();
		reinterpret_cast<VK_Texture*>(mTexture)->DestroyImage();
	}
}