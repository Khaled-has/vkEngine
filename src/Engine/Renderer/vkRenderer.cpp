#include "vkRenderer.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <Window.h>

namespace Render
{

	vkRenderer* vkRenderer::m_pInstance;

	static ImTextureID images[3];

	void vkRenderer::Initialize()
	{
		m_pInstance = this;

		// # Device: Create the surface & initialize
		m_Device.Initialize();

		// # Swapchain: initialize
		m_SwapChain.Create(VK_SwapChainInfo{ .mEnableDepth = false, .mEnableVSync = true });

		// # Command pool & Command buffers: creation
		m_pCmdPool.Create(m_Device.getQueues().mGraphics->getQFamilyIndex());

		m_pCmdBufs.resize(m_SwapChain.getSwapChainImageCount());
		m_pCmdPool.AllocCmdBufs((uint32_t)m_pCmdBufs.size(), m_pCmdBufs.data());

		m_pCmdPool.AllocCmdBufs(1u, &m_CopyCmdBuf);

		// # Initialize render graph
		InitRenderGraph();

		// # Initialize screen view > ( Set last frame from the render graph )
		InitScreenView(m_pRenderGraph->getImage("Color_Attach"));

		// # ImGui Initialization
		m_ImGuiRenderer.Initialize(m_Device, m_SwapChain, m_pCmdPool);

		// # Record the command buffers
		RecordCommandBuffers();

		// # Semaphores manager: create
		m_pSemManager.Create(m_SwapChain.getSwapChainImageCount());
	}

	void vkRenderer::Destroy()
	{
		mScreenView.mBuffer.Destroy();
		m_ImGuiRenderer.Destroy();
		m_pRenderGraph->Destroy();

		m_pSemManager.Destroy();

		m_pCmdPool.FreeCmdBufs((uint32_t)m_pCmdBufs.size(), m_pCmdBufs.data());
		m_pCmdPool.FreeCmdBufs(1u, &m_CopyCmdBuf);
		m_pCmdPool.Destroy();

		m_SwapChain.Destroy();

		m_Device.Destroy();
	}

	void vkRenderer::Rendering()
	{
		// # ImGui render
		m_ImGuiRenderer.Render();

		// # Acquired next image
		CHECK_VK_RES(m_pSemManager.AcquireNextImage(m_SwapChain), "vkAcquiredNextImage");
		uint32_t mAcqImageIndex = m_pSemManager.getAcquiredImageIndex();

		// # Submit queue
		const VK_CmdBuf CmdBufs[] = {
			m_pCmdBufs[mAcqImageIndex],
			m_ImGuiRenderer.getCmdBuf(mAcqImageIndex, [](
				const VkCommandBuffer& CmdBuf, VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount
				) {
					ImageMemBarrier(CmdBuf, Image, Format, OldLayout, NewLayout, LayerCount);
				}
			)
		};

		m_pSemManager.SubmitAsync(
			&CmdBufs[0], uint32_t(sizeof(CmdBufs) / sizeof(VK_CmdBuf)),
			*m_Device.getQueues().mPresent
		);

		// # Present
		VkResult res = m_pSemManager.Present(mAcqImageIndex, m_SwapChain, *m_Device.getQueues().mPresent);

		// # Window changes check
		static bool isWindowChanged = false;
		static bool isWindowHidden = false;

		if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
		{
			isWindowChanged = true;

			VkSurfaceCapabilitiesKHR Caps;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device.getSelectedPhysDevice().m_pPhysDev, m_Device.getSurface(), &Caps);

			if (Caps.currentExtent.width <= 0 || Caps.currentExtent.height <= 0)
				isWindowHidden = true;
		}

		// # Recreate swapchain if the window size changed
		if (isWindowChanged && !isWindowHidden)
		{
			vkDeviceWaitIdle(m_Device.getDevice());

			m_SwapChain.Destroy();

			m_SwapChain.Create(VK_SwapChainInfo{ .mEnableDepth = false, .mEnableVSync = true });

			RecordCommandBuffers();

			isWindowChanged = false;
		}
	}

	void vkRenderer::AddViewport(const Viewport& mViewport)
	{
	}

	void vkRenderer::RecordCommandBuffers()
	{
		// # Draw command: recording
		for (uint32_t i = 0; i < 3; i++)
		{
			BeginCmdBuf(m_pCmdBufs[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

			// # Run the render graph passes' draw calls
			m_pRenderGraph->Run(m_pCmdBufs[i], i);

			// # Barrier last render graph's frame to read from it
			m_pRenderGraph->PrepareToRead("Color_Attach", m_pCmdBufs[i], i);
			// # Draw screen image quad
			ScreenViewDraw(m_pCmdBufs[i], i);

			CHECK_VK_RES(vkEndCommandBuffer(m_pCmdBufs[i].getHandle()), "vkEndCommandBuffer");
		}
	}

	void vkRenderer::InitRenderGraph()
	{
		m_pRenderGraph = std::make_unique<RenderGraph>(m_Device, m_CopyCmdBuf);

		m_pRenderGraph->AddPass(
			"DrawColorPass",
			[](Pass& mSetup) {
				mSetup.AddColorAttachment("Color_Attach", VK_FORMAT_R8G8B8A8_UNORM);
				mSetup.WritesImage("Color_Attach");
			},
			[](PassInitialization& mInit)
			{
				DrawColorPass::Instance()->Initialize(mInit.mDevice, mInit.mCmdBuf, mInit.getImage("Color_Attach").mImages.data(), 3);
			},
			[](PassExecution& mRun)
			{
				DrawColorPass::Instance()->Run(mRun.mCmdBuf, mRun.mFrameIndex);
			}
		);

		m_pRenderGraph->Setup();
	}

	void vkRenderer::InitScreenView(Image& mGraphImage)
	{
		// # Create: render pass
		auto window_info = Window::getInstance()->getWindowInfo();
		std::vector<VK_PassInfo> passInfos;
		for (uint32_t i = 0; i < m_SwapChain.getSwapChainImageCount(); i++)
		{
			passInfos.push_back(VK_PassInfo{
				.pColorTextures = &m_SwapChain.getSwapChainTextures()[i],
				.mColorCount = 1,
				.pClearColorValues = new VkClearValue{.color = { 0, 0, 0, 1 } },

				.pDepthTexture = NULL,

				.mRenderArea = VkRect2D{.offset{.x = 0, .y = 0}, .extent{.width = window_info.mWidth, .height = window_info.mHeight}},
				.mIsColor = true,
				.mIsDepth = false
				});
		}

		mScreenView.mRenderPass.Create(passInfos);

		// # Create: shader
		const char* vertex_shader = R"(
		#version 460

		layout (location = 0) out vec2 mUVs;

		vec2 uvs[] = {
			vec2(0.0, 0.0), vec2(1.0, 0.0),
			vec2(1.0, 1.0), vec2(0.0, 1.0)
		};

		uint indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		struct VertexData
		{
			float x, y;
		};
		
		layout (binding = 0) readonly buffer VertexBuffer { VertexData data[]; } vertices;
		
		void main()
		{
			VertexData vtx = vertices.data[indices[gl_VertexIndex]];
			mUVs = uvs[indices[gl_VertexIndex]];
		
			gl_Position = vec4(vtx.x, vtx.y, 0.0, 1.0);
		} 
	)";

		const char* fragment_shader = R"(
		#version 460

		layout (location = 0) out vec4 out_Color;
		layout (location = 0) in vec2 mUVs;

		layout (binding = 1) uniform sampler2D mTexture;
		
		void main()
		{
		    out_Color = texture(mTexture, mUVs);
		}
	)";

		mScreenView.mShader.CreateFromText(vertex_shader, fragment_shader);

		// # Create: buffer
		const std::vector<float> pVertices = {
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			 1.0f, 1.0f,
			-1.0f, 1.0f
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

		mScreenView.mBuffer.Create(vertexBufferInfo, vertexAllocInfo);

		BeginCmdBuf(m_CopyCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferCopy copyRegion = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = bufferSize
		};

		vkCmdCopyBuffer(m_CopyCmdBuf.getHandle(), vertexStageBuffer.getBuffer(), mScreenView.mBuffer.getBuffer(), 1, &copyRegion);
		vkEndCommandBuffer(m_CopyCmdBuf.getHandle());

		m_Device.getQueues().mTransfer->SubmitSync(&m_CopyCmdBuf.getHandle(), 1);
		vkQueueWaitIdle(m_Device.getQueues().mTransfer->getHandle());
		vertexStageBuffer.Destroy();

		// # Create: graphics pipeline
		VK_Binding bindings[] = {
			{
				.mBind = 0,
				.mType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.mDescCount = 1,
				.mStage = VK_SHADER_STAGE_VERTEX_BIT,

				.mBufCount = 1,
				.mBuffer = &mScreenView.mBuffer
			},
			{
				.mBind = 1,
				.mType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.mDescCount = 1,
				.mStage = VK_SHADER_STAGE_FRAGMENT_BIT,

				.mTexCount = (uint32_t)mGraphImage.mImages.size(),
				.mTexture = mGraphImage.mImages.data()
			}
		};

		mScreenView.mPipeline.Create(m_SwapChain.getSwapChainImageCount(), bindings);

		VK_Attachment attachments[] = {
			{
				.mFormat = m_SwapChain.getColorImageFormat(),
				.mIsColorAttach = true,
				.mIsDepthAttach = false
			}
		};

		VkViewport viewPort = {
			.x = 0, .y = 0,
			.width = (float)window_info.mWidth, .height = (float)window_info.mHeight,
			.minDepth = 1.0, .maxDepth = 0.0,
		};

		VkRect2D scissor = {
			.offset{.x = 0, .y = 0},
			.extent{.width = window_info.mWidth, .height = window_info.mHeight}
		};

		mScreenView.mPipeline.CreatePipeline(mScreenView.mShader, attachments, viewPort, scissor);
	}

	void vkRenderer::ScreenViewDraw(VK_CmdBuf& mCmdBuf, uint32_t mIndex)
	{
		// # Swapchain image barrier to COLOR_ATTACHMENT
		TextureLayoutTransition(m_SwapChain.getSwapChainTextures()[mIndex], mCmdBuf, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		mScreenView.mRenderPass.Begin(mCmdBuf, mIndex);

		// # Draw screen image quad
		mScreenView.mPipeline.Bind(mIndex, mCmdBuf);
		vkCmdDraw(mCmdBuf.getHandle(), 6, 1, 0, 0);

		mScreenView.mRenderPass.End(mCmdBuf, mIndex);
	}
}