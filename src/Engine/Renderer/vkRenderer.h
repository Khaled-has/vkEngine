#pragma once

#include "Renderer.h"
#include "vkGPU.h"

#include "RenderGraph.h"

#include "imgui/ImGuiRenderer.h"
#include "DrawColorPass.h"

namespace Render
{
	class vkRenderer : public Renderer
	{
	public:
		vkRenderer() {}

		static vkRenderer* Instance() { return m_pInstance; }

		VK_Device& getDevice() { return m_Device; }
		VK_SwapChain& getSwapChain() { return m_SwapChain; }

		virtual void Initialize() override;
		virtual void Destroy() override;

		virtual void Rendering() override;

		virtual void AddViewport(const Viewport& mViewport) override;

	private:
		static vkRenderer* m_pInstance;

		ImGuiRenderer m_ImGuiRenderer;

		// # Screen view
		struct
		{
			VK_GraphPipeline mPipeline;
			VK_RenderPass mRenderPass;

			VK_Buffer mBuffer;
			VK_Shader mShader;

		} mScreenView;

		VK_Device m_Device;
		VK_SwapChain m_SwapChain;

		VK_CmdPool m_pCmdPool;
		std::vector<VK_CmdBuf> m_pCmdBufs;
		VK_CmdBuf m_CopyCmdBuf;

		VK_SemaphoresManager m_pSemManager;

		std::unique_ptr<RenderGraph> m_pRenderGraph;

		void RecordCommandBuffers();

		void InitRenderGraph();

		void InitScreenView(Image& mGraphImage);
		void ScreenViewDraw(VK_CmdBuf& mCmdBuf, uint32_t mIndex);
	};
}