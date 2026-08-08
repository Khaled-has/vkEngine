#include "vkViewport.h"
#include "vkRenderer.h"

#include "DrawColorPass.h"

#include <imgui_impl_vulkan.h>

namespace Render
{
	void vkViewport::Create(ViewportInfo mInfo)
	{
		std::cout << "Hi> 1\n";
		VK_CmdBuf* cmd = new VK_CmdBuf;
		vkRenderer::Instance()->getCmdPool().AllocCmdBufs(1, cmd);
		std::cout << "Hi> 2\n";
		m_pRenderGraph = std::make_unique<RenderGraph>(vkRenderer::Instance()->getDevice(), *cmd);

		DrawColorPass mDrawColorPass;

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

		m_CmdBufs.resize(3);
		vkRenderer::Instance()->getCmdPool().AllocCmdBufs((uint32_t)m_CmdBufs.size(), m_CmdBufs.data());

		RecordCmdBufs();

		m_DescSets.resize(3);
		for (uint32_t i = 0; i < m_DescSets.size(); i++)
		{
			VK_Texture& tex = m_pRenderGraph->getImage("Color_Attach").mImages[i];
			m_DescSets[i] = ImGui_ImplVulkan_AddTexture(tex.getSampler(), tex.getView(), tex.getLayout());
		}

		std::cout << "Hello world\n";
	}

	void vkViewport::Destroy()
	{
	}

	ImTextureID vkViewport::getImageView()
	{
		return (ImTextureID)m_DescSets[vkRenderer::Instance()->getImageIndex()];
	}

	void vkViewport::RecordCmdBufs()
	{
		for (uint32_t i = 0; i < m_CmdBufs.size(); i++)
		{
			BeginCmdBuf(m_CmdBufs[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

			// # Run the render graph passes' draw calls
			m_pRenderGraph->Run(m_CmdBufs[i], i);

			// # Barrier last render graph's frame to read from it
			m_pRenderGraph->PrepareToRead("Color_Attach", m_CmdBufs[i], i);

			CHECK_VK_RES(vkEndCommandBuffer(m_CmdBufs[i].getHandle()), "vkEndCommandBuffer");
		}
	}
}