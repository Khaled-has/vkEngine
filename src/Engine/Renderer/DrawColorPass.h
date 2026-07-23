#pragma once

#include "vkGPU.h"

#include <span>

class DrawColorPass
{
public:
	DrawColorPass() { pInstance = this; }
	~DrawColorPass() { }

	void Destroy(){ m_pVertexBuffer.Destroy(); }

	void Initialize(const VK_Device& mDevice, const VK_CmdBuf& mCopyCmdBuf, VK_Texture* pImages, uint32_t mCount);
	

	inline void Run(VK_CmdBuf& mCmdBuf, uint32_t mFrameIndex) 
	{
		m_pRenderPass.Begin(mCmdBuf, mFrameIndex);
		// ..

		m_pPipeline.Bind(mFrameIndex, mCmdBuf);
		vkCmdDraw(mCmdBuf.getHandle(), 3, 1, 0, 0);

		// ..
		m_pRenderPass.End(mCmdBuf, mFrameIndex);
	}

	static DrawColorPass* Instance() { return pInstance; }

private:
	inline static DrawColorPass* pInstance;

	VK_Shader m_pShader;

	VK_GraphPipeline m_pPipeline;

	VK_RenderPass m_pRenderPass;
	std::vector<VK_PassInfo> mPassInfos;

	VK_Buffer m_pVertexBuffer;
	VK_Buffer m_pIndexBuffer;

	void CreateBuffers(const VK_Device& mDevice, const VK_CmdBuf& mCopyCmdBuf);
	
};