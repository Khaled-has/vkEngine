#include "DrawColorPass.h"

void DrawColorPass::Initialize(const VK_Device& mDevice, const VK_CmdBuf& mCopyCmdBuf, VK_Texture* pImages, uint32_t mCount)
{
	// # Screen render pass: initialize
	for (uint32_t i = 0; i < mCount; i++)
	{
		mPassInfos.push_back(VK_PassInfo{
			.pColorTextures = &pImages[i],
			.mColorCount = 1,
			.pClearColorValues = new VkClearValue{.color = { 0.1, 0.1, 0, 1 } },

			.pDepthTexture = NULL,

			.mRenderArea = VkRect2D{.offset{.x = 0, .y = 0}, .extent{.width = 1440, .height = 720}},
			.mIsColor = true,
			.mIsDepth = false
			});
	}

	m_pRenderPass.Create(mPassInfos);

	// Create buffers
	CreateBuffers(mDevice, mCopyCmdBuf);

	// Create shader
	m_pShader.Create("shaders/vert.vs", "shaders/frag.fs");

	// Create pipeline
	VK_Binding bindings[] = {
		{
			.mBind = 0,
			.mType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.mDescCount = 1,
			.mStage = VK_SHADER_STAGE_VERTEX_BIT,

			.mBufCount = 1,
			.mBuffer = &m_pVertexBuffer
		}
	};

	m_pPipeline.Create(3, bindings);

	VK_Attachment attachments[] = {
		{
			.mFormat = pImages[0].getFormat(),
			.mIsColorAttach = true,
			.mIsDepthAttach = false
		}
	};
	VkViewport viewPort = {
		.x = 0, .y = 0,
		.width = 1440, .height = 720,
		.minDepth = 1.0, .maxDepth = 0.0,
	};
	m_pPipeline.CreatePipeline(m_pShader, attachments, viewPort, VkRect2D{ .offset{.x = 0, .y = 0}, .extent{.width = 1440, .height = 720} });
}

void DrawColorPass::CreateBuffers(const VK_Device& mDevice, const VK_CmdBuf& mCopyCmdBuf)
{
	// # Test create vertex & index buffers
	const std::vector<float> pVertices = {
		-0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -0.5f
	};

	VkDeviceSize bufferSize = pVertices.size() * sizeof(float);

	VkBufferCreateInfo stageBufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = bufferSize,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	};

	VmaAllocationCreateInfo stageAllocInfo = {
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	VK_Buffer vertexStageBuffer;
	vertexStageBuffer.Create(stageBufferInfo, stageAllocInfo);

	memcpy(vertexStageBuffer.getAllocInfo().pMappedData, pVertices.data(), bufferSize);

	VkBufferCreateInfo vertexBufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = bufferSize,
		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
	};

	VmaAllocationCreateInfo vertexAllocInfo = {
		.usage = VMA_MEMORY_USAGE_AUTO,
		.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	};

	m_pVertexBuffer.Create(vertexBufferInfo, vertexAllocInfo);

	BeginCmdBuf(mCopyCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = bufferSize
	};

	vkCmdCopyBuffer(mCopyCmdBuf.getHandle(), vertexStageBuffer.getBuffer(), m_pVertexBuffer.getBuffer(), 1, &copyRegion);
	vkEndCommandBuffer(mCopyCmdBuf.getHandle());

	mDevice.getQueues().mTransfer->SubmitSync(&mCopyCmdBuf.getHandle(), 1);
	vkQueueWaitIdle(mDevice.getQueues().mTransfer->getHandle());
	vertexStageBuffer.Destroy();
}
