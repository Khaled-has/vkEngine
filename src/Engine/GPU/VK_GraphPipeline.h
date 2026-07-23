#ifndef VK_GRAPHPIPELINE_H
#define VK_GRAPHPIPELINE_H

#include <vector>
#include <span>

#include "VK_CmdBuf.h"
#include "VK_Buffer.h"
#include "VK_Texture.h"
#include "VK_Shader.h"

struct VK_Binding
{
	uint32_t mBind;
	VkDescriptorType mType;
	uint32_t mDescCount;
	VkShaderStageFlags mStage;

	uint32_t mBufCount;
	const VK_Buffer* mBuffer;
	uint32_t mTexCount;
	const VK_Texture* mTexture;

};

struct VK_Attachment
{
	VkFormat mFormat;
	bool mIsColorAttach;
	bool mIsDepthAttach;
	
};

class VK_GraphPipeline
{
public:
	VK_GraphPipeline() {}

	void Create(uint32_t mImageCount, std::span<VK_Binding> mBindings);
	void Destroy();
	
	void CreatePipeline(
		const VK_Shader& pShader, std::span<VK_Attachment> mAttachments,
		VkViewport mViewport, VkRect2D mScissor
	);

	void Bind(uint32_t mIndex, const VK_CmdBuf& mCmdBuf);

private:
	VkPipeline m_pPipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_pPipLayout = VK_NULL_HANDLE;

	std::vector<VkDescriptorSet> m_pDescSets;
	
	VkDescriptorSetLayout m_pDescSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_pDescPool = VK_NULL_HANDLE;

	void CreateDescriptorPool(uint32_t mImageCount, std::span<VK_Binding> mBindings);
	void CreateDescriptorLayout(uint32_t mImageCount, std::span<VK_Binding> mBindings);
	void AllocateDescriptorSets(uint32_t mImageCount);
	void UpdateDescriptorSets(uint32_t mImageCount, std::span<VK_Binding> mBindings);
};

#endif