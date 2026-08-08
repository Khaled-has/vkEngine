#ifndef VKGRAPHICSPIPELINE_H
#define VKGRAPHICSPIPELINE_H

#include "vkShader.h"
#include "vkBuffer.h"
#include "vkTexture.h"

namespace GPU
{
	struct vkBinding
	{
		uint32_t mBinding;
		VkDescriptorType mDescType;
		uint32_t mDescCount;
		VkShaderStageFlags mStage;
		std::span<const vkTexture> mTextures;
		std::span<const vkBuffer> mBuffers;
	};

	typedef enum vkAttachmentType
	{
		GPU_COLOR_ATTACH = 1,
		GPU_DEPTH_ATTACH = 2
	};

	struct vkAttachment
	{
		VkFormat mFormat;
		vkAttachmentType mAttachType;
	};

	struct vkPipelineInfo
	{
		std::span<const vkAttachment> mAttachments;
		vkShader mShader;
		VkViewport mViewport;
		VkRect2D mScissor;
		VkPipelineBindPoint mBindPoint;
	};

	class VKGPU_API vkGraphicsPipeline
	{
	public:
		vkGraphicsPipeline() {}

		void CreatePipeline(const vkPipelineInfo& mInfo);
		void UpdateBindings(std::span<const vkBinding> mBindings, uint32_t mImageCount);

		void DestroyPipeline();
		void DestroyBindings();

		void Bind(const vkCmdBuf& mCmdBuf, uint32_t mIndex);

	private:
		bool m_PipelineCreated			= false;
		VkPipeline m_Pipeline			= VK_NULL_HANDLE;
		VkPipelineLayout m_PipLayout	= VK_NULL_HANDLE;

		VkPipelineBindPoint m_PipBindPoint;

		bool m_BindingsCreated					= false;
		std::vector<VkDescriptorSet> m_DescSets;
		VkDescriptorSetLayout m_DescSetLayout	= VK_NULL_HANDLE;
		VkDescriptorPool m_DescPool				= VK_NULL_HANDLE;

		void CreateDescPool(std::span<const vkBinding> mBindings, uint32_t mImageCount);
		void CreateDescLayout(std::span<const vkBinding> mBindings, uint32_t mImageCount);
		void AllocateDescSets(uint32_t mImageCount);
		void UpdateDescSets(std::span<const vkBinding> mBindings, uint32_t mImageCount);
	};
}

#endif