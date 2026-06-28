#include "renderer.h"

void renderer::Initialize(SDL_Window* pWin)
{
	pAppWin = pWin;

	// # Device: Create the surface & initialize
	m_pDevice.Initialize([pWin](VkSurfaceKHR* pSurface, VkInstance mInstance) {
		if (!SDL_Vulkan_CreateSurface(pWin, mInstance, NULL, pSurface))
		{
			LOG_ERROR("Vulkan Error: SDL_VulkanCreateSurface");
		}
	});

	// # Swapchain: initialize
	m_pSwapChain.Create(VK_SwapChainInfo{ .mEnableDepth = false, .mEnableVSync = true });

	// # Screen render pass: initialize
	std::vector<VK_PassInfo> mPassInfos(m_pSwapChain.getSwapChainImageCount());
	VkClearValue* ClearColor = new VkClearValue{ .color = { 1, 0, 0, 1 } };
	for (uint32_t i = 0; i < mPassInfos.size(); i++)
	{
		mPassInfos[i] = {
			.pColorTextures = &m_pSwapChain.getSwapChainTextures()[i],
			.mColorCount = 1,
			.pClearColorValues = ClearColor,

			.pDepthTexture = NULL,

			.mInitLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.mNewLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.mFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

			.mRenderArea = VkRect2D{.offset{.x = 0, .y = 0}, .extent{.width = 1440, .height = 720}},
			.mIsColor = true,
			.mIsDepth = false
		};
	}

	m_pRenderPass.Create(mPassInfos.data(), (uint32_t)mPassInfos.size());
	
	// # Command pool & Command buffers: creation
	m_pCmdPool.Create(m_pDevice.getQueues().mGraphics->getQFamilyIndex());
	
	m_pCmdBufs.resize(m_pSwapChain.getSwapChainImageCount());
	m_pCmdPool.AllocCmdBufs((uint32_t)m_pCmdBufs.size(), m_pCmdBufs.data());

	m_pCmdPool.AllocCmdBufs(1u, &m_pCopyCmdBuf);

	// Create buffers
	CreateBuffers();

	// Create pipeline
	const VK_Binding bindings = {
		.mBind = 0,
		.mType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.mDescCount = 1,
		.mStage = VK_SHADER_STAGE_VERTEX_BIT,

		.mBufCount = 1,
		.mBuffer = &m_pVertexBuffer,
	};

	m_pPipeline.Create(3, &bindings, 1);

	m_pShader.Create("shaders/vert.vs", "shaders/frag.fs");

	const VK_Attachment attach = {
		.mFormat = m_pSwapChain.getColorImageFormat(),
		.mIsColorAttach = true,
		.mIsDepthAttach = false,
	};
	VkViewport viewPort = {
		.x = 0, .y = 0,
		.width = 1440, .height = 720,
		.minDepth = 1.0, .maxDepth = 0.0,
	};
	m_pPipeline.CreatePipeline(m_pShader, &attach, 1, viewPort, VkRect2D{ .offset{.x = 0, .y = 0}, .extent{.width = 1440, .height = 720} });
	
	// # Draw command: recording
	for (uint32_t i = 0; i < m_pCmdBufs.size(); i++)
	{
		BeginCmdBuf(m_pCmdBufs[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		m_pRenderPass.Begin(m_pCmdBufs[i], i);
		// ..
		m_pPipeline.Bind(i, m_pCmdBufs[i]);
		vkCmdDraw(m_pCmdBufs[i].getHandle(), 3, 1, 0, 0);
		// ..
		m_pRenderPass.End(m_pCmdBufs[i], i);

		CHECK_VK_RES(vkEndCommandBuffer(m_pCmdBufs[i].getHandle()), "vkEndCommandBuffer");
	}

	// # Semaphores manager: create
	m_pSemManager.Create(m_pSwapChain.getSwapChainImageCount());
}

void renderer::render()
{
	// # Acquired next image
	CHECK_VK_RES(m_pSemManager.AcquireNextImage(m_pSwapChain), "vkAcquiredNextImage");
	uint32_t mAcqImageIndex = m_pSemManager.getAcquiredImageIndex();

	// # Submit queue
	m_pSemManager.SubmitAsync(
		&m_pCmdBufs[mAcqImageIndex], 1, 
		*m_pDevice.getQueues().mPresent
	);

	// # Present
	CHECK_VK_RES(
		m_pSemManager.Present(mAcqImageIndex, m_pSwapChain, *m_pDevice.getQueues().mPresent),
		"vkPresentKHR"
	);
}

void renderer::CreateBuffers()
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
	
	BeginCmdBuf(m_pCopyCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = bufferSize
	};

	vkCmdCopyBuffer(m_pCopyCmdBuf.getHandle(), vertexStageBuffer.getBuffer(), m_pVertexBuffer.getBuffer(), 1, &copyRegion);
	vkEndCommandBuffer(m_pCopyCmdBuf.getHandle());

	m_pDevice.getQueues().mTransfer->SubmitSync(&m_pCopyCmdBuf.getHandle(), 1);
	vkQueueWaitIdle(m_pDevice.getQueues().mTransfer->getHandle());
}
