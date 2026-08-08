#pragma once

#include "RenderGraph.h"

#include "Viewport.h"

namespace Render
{
	//class vkViewport : public Viewport
	//{
	//public:
	//	vkViewport() {}

	//	virtual void Create(ViewportInfo mInfo) override;
	//	virtual void Destroy() override;

	//	virtual ImTextureID getImageView() override;

	//	inline VK_CmdBuf& getCmdBufs(uint32_t mIndex) { return m_CmdBufs[mIndex]; }

	//private:
	//	std::unique_ptr<RenderGraph> m_pRenderGraph;

	//	std::vector<VK_CmdBuf> m_CmdBufs;
	//	std::vector<VkDescriptorSet> m_DescSets;

	//	void RecordCmdBufs();
	//};
}