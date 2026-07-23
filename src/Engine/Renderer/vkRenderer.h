#pragma once

#include "Renderer.h"
#include "vkGPU.h"

#include "RenderGraph.h"

#include "imgui/ImGuiRenderer.h"
#include "DrawColorPass.h"

class vkRenderer : public Renderer
{
public:
	vkRenderer() {}

	static vkRenderer* Instance() { return m_pInstance; }

	virtual void Initialize() override;
	virtual void Destroy() override;
	
	virtual void Rendering() override;

private:
	static vkRenderer* m_pInstance;

	ImGuiRenderer m_ImGuiRenderer;

	// # Passes
	DrawColorPass mDrawColorPass;


	// # Screen view
	struct
	{
		VK_GraphPipeline mPipeline;
		VK_RenderPass mRenderPass;

		VK_Buffer mBuffer;
		VK_Shader mShader;

	} mScreenView;

	VK_Device m_Device;
	VK_SwapChain m_pSwapChain;

	VK_CmdPool m_pCmdPool;
	std::vector<VK_CmdBuf> m_pCmdBufs;
	VK_CmdBuf m_CopyCmdBuf;

	VK_SemaphoresManager m_pSemManager;

	std::unique_ptr<RenderGraph> m_pRenderGraph;
	
	void InitRenderGraph();
	
	void InitScreenView(Image& mGraphImage);
	void ScreenViewDraw(VK_CmdBuf& mCmdBuf, uint32_t mIndex);
};