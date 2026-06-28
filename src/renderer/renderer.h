#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "vk/VK_Header.h"

class renderer
{
public:
	renderer() {}

	void Initialize(SDL_Window* pWin);

	void render();

private:
	SDL_Window* pAppWin;

	VK_Device m_pDevice;
	VK_SwapChain m_pSwapChain;

	VK_CmdPool m_pCmdPool;
	std::vector<VK_CmdBuf> m_pCmdBufs;
	VK_CmdBuf m_pCopyCmdBuf;

	VK_Shader m_pShader;

	VK_GraphPipeline m_pPipeline;
	VK_RenderPass m_pRenderPass;

	VK_SemaphoresManager m_pSemManager;

	VK_Buffer m_pVertexBuffer;
	VK_Buffer m_pIndexBuffer;

	void CreateBuffers();
};

#endif