#include "vkGraphicsPipeline.h"
#include "vkContext.h"
#include "vkLog.h"

namespace GPU
{
    void vkGraphicsPipeline::CreatePipeline(const vkPipelineInfo& mInfo)
    {
		if (m_PipelineCreated)
		{
			DestroyPipeline();
			m_PipelineCreated = false;
			GPU_LOG_WARN("vkGPU: You create the pipeline when he's already created that's destroy the old pipeline and recreate it");
		}

		m_PipBindPoint = mInfo.mBindPoint;

		// # Create the pipeline
		VkPipelineShaderStageCreateInfo ShaderStagesCreateInfo[2] = {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = mInfo.mShader.getVertexModule(),
				.pName = "main"
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = mInfo.mShader.getFragmentModule(),
				.pName = "main"
			} 
		};

		VkPipelineVertexInputStateCreateInfo VertexInputInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		};

		VkPipelineInputAssemblyStateCreateInfo IACreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};

		VkPipelineViewportStateCreateInfo ViewportCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &mInfo.mViewport,
			.scissorCount = 1,
			.pScissors = &mInfo.mScissor
		};

		VkPipelineRasterizationStateCreateInfo RastCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.lineWidth = 1.0f
		};

		VkPipelineMultisampleStateCreateInfo MSCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f
		};

		VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
			.front = {},
			.back = {},
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f
		};

		VkPipelineColorBlendAttachmentState BlendAttachState = {
			.blendEnable = VK_FALSE,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
		};

		VkPipelineColorBlendStateCreateInfo BlendCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &BlendAttachState
		};

		std::vector<VkFormat> ColorFormats;
		VkFormat DepthFormat = VK_FORMAT_UNDEFINED;

		for (uint32_t i = 0; i < mInfo.mAttachments.size(); i++)
		{
			const auto& attach = mInfo.mAttachments[i];
			if (attach.mAttachType == vkAttachmentType::GPU_COLOR_ATTACH)
			{
				ColorFormats.push_back(attach.mFormat);
			}
			else if (attach.mAttachType == vkAttachmentType::GPU_DEPTH_ATTACH)
			{
				DepthFormat = attach.mFormat;
			}
		}
		VkPipelineRenderingCreateInfo RenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			.pNext = NULL,
			.colorAttachmentCount = (uint32_t)ColorFormats.size(),
			.pColorAttachmentFormats = ColorFormats.data(),
			.depthAttachmentFormat = DepthFormat,
			.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
		};

		VkPipelineLayoutCreateInfo LayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &m_DescSetLayout
		};

		const VkDevice& device = GetCurrentContext()->getDeviceRes().pDevice->getHandle();

		GPU_CHECK(vkCreatePipelineLayout(device, &LayoutInfo, NULL, &m_PipLayout), "vkCreatePipelineLayout");

		VkGraphicsPipelineCreateInfo GPipelineInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &RenderingInfo,
			.stageCount = (sizeof(ShaderStagesCreateInfo) / sizeof(VkPipelineShaderStageCreateInfo)),
			.pStages = &ShaderStagesCreateInfo[0],
			.pVertexInputState = &VertexInputInfo,
			.pInputAssemblyState = &IACreateInfo,
			.pViewportState = &ViewportCreateInfo,
			.pRasterizationState = &RastCreateInfo,
			.pMultisampleState = &MSCreateInfo,
			.pDepthStencilState = &DepthStencilCreateInfo,
			.pColorBlendState = &BlendCreateInfo,
			.layout = m_PipLayout,
			.renderPass = nullptr,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};

		GPU_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &GPipelineInfo, NULL, &m_Pipeline), "vkCreateGraphicsPipelines");

		m_PipelineCreated = true;
    }

    void vkGraphicsPipeline::UpdateBindings(std::span<const vkBinding> mBindings, uint32_t mImageCount)
    {
		if (!m_PipelineCreated)
		{
			GPU_LOG_ERROR("vkGPU: You can't update the bindings the vkGraphicsPipeline before you create the pipeline");
			exit(1);
		}

		// # Destroy the bindings before we trying to reupdate it
		if (m_BindingsCreated)
		{
			DestroyBindings();
		}

		CreateDescPool(mBindings, mImageCount);
		CreateDescLayout(mBindings, mImageCount);
		AllocateDescSets(mImageCount);
		UpdateDescSets(mBindings, mImageCount);

		m_BindingsCreated = true;
    }

	void vkGraphicsPipeline::DestroyPipeline()
	{
		m_PipelineCreated = false;
	}

	void vkGraphicsPipeline::DestroyBindings()
    {
		// # Check
		if (!m_BindingsCreated)
		{
			GPU_LOG_WARN("vkGPU: You can't destroy the bindings before you create it");
			return;
		}

		m_BindingsCreated = false;
    }

    void vkGraphicsPipeline::Bind(const vkCmdBuf& mCmdBuf, uint32_t mIndex)
    {
		// # Check the bindings
		if (!m_BindingsCreated)
		{
			GPU_LOG_ERROR("vkGPU: You can't call vkGraphicsPipeline->Bind() func before you update the bindings");
			exit(1);
		}
		// # Check the pipeline
		if (!m_PipelineCreated)
		{
			GPU_LOG_ERROR("vkGPU: You can't call vkGraphicsPipeline->Bind() func before you Create the pipeline");
			exit(1);
		}

		// 1 # Bind pipeline
		vkCmdBindPipeline(
			mCmdBuf.getHandle(), m_PipBindPoint, m_Pipeline
		);

		// 2 # Bind descriptor sets
		vkCmdBindDescriptorSets(
			mCmdBuf.getHandle(), m_PipBindPoint, m_PipLayout,
			0, 1,
			&m_DescSets[mIndex], 0, NULL
		);
    }

    void vkGraphicsPipeline::CreateDescPool(std::span<const vkBinding> mBindings, uint32_t mImageCount)
    {
		std::vector<VkDescriptorPoolSize> PoolSizes;

		for (const auto& Bind : mBindings)
		{
			PoolSizes.push_back(
				VkDescriptorPoolSize{
					.type = Bind.mDescType,
					.descriptorCount = Bind.mDescCount
				}
			);
		}

		VkDescriptorPoolCreateInfo PoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = mImageCount,
			.poolSizeCount = (uint32_t)PoolSizes.size(),
			.pPoolSizes = PoolSizes.data()
		};

		GPU_CHECK(vkCreateDescriptorPool(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &PoolInfo, NULL, &m_DescPool), 
			"vkCreateDescriptorPool"
		);
    }

	void vkGraphicsPipeline::CreateDescLayout(std::span<const vkBinding> mBindings, uint32_t mImageCount)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (const auto& Bind : mBindings)
		{
			bindings.push_back(
				VkDescriptorSetLayoutBinding{
					.binding = Bind.mBinding,
					.descriptorType = Bind.mDescType,
					.descriptorCount = Bind.mDescCount,
					.stageFlags = Bind.mStage
				}
			);
		}

		VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = (uint32_t)bindings.size(),
			.pBindings = bindings.data()
		};

		GPU_CHECK(
			vkCreateDescriptorSetLayout(GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &CreateInfo, NULL, &m_DescSetLayout),
			"vkCreateDescriptorSetLayout"
		);
	}

	void vkGraphicsPipeline::AllocateDescSets(uint32_t mImageCount)
	{
		std::vector<VkDescriptorSetLayout> layouts(mImageCount, m_DescSetLayout);

		VkDescriptorSetAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = m_DescPool,
			.descriptorSetCount = mImageCount,
			.pSetLayouts = layouts.data()
		};

		m_DescSets.resize(mImageCount);

		GPU_CHECK(vkAllocateDescriptorSets(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &AllocInfo, m_DescSets.data()),
			"vkAllocateDescriptorSets"
		);
	}

	void vkGraphicsPipeline::UpdateDescSets(std::span<const vkBinding> mBindings, uint32_t mImageCount)
	{
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			std::vector<VkWriteDescriptorSet> WriteDescSets{};

			for (uint32_t j = 0; j < mBindings.size(); j++)
			{
				// # Buffers
				if (mBindings[j].mBuffers.size() != 0)
				{
					VkDescriptorBufferInfo BufferInfo = {
						.buffer = mBindings[j].mBuffers.size() > 1 ? mBindings[j].mBuffers[i].getBuffer() : mBindings[j].mBuffers[0].getBuffer(),
						.offset = 0,
						.range = VK_WHOLE_SIZE
					};

					WriteDescSets.push_back(
						VkWriteDescriptorSet{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = m_DescSets[i],
							.dstBinding = mBindings[j].mBinding,
							.dstArrayElement = 0,
							.descriptorCount = mBindings[j].mDescCount,
							.descriptorType = mBindings[j].mDescType,
							.pBufferInfo = &BufferInfo,
						}
						);
				}
				// # Texture
				else if (mBindings[j].mTextures.size() != 0)
				{
					const vkTexture& CurTexture = mBindings[j].mTextures.size() > 1 ? mBindings[j].mTextures[i] : mBindings[j].mTextures[0];

					VkDescriptorImageInfo ImageInfo = {
						.sampler = CurTexture,
						.imageView = CurTexture,
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL // TODO: Hard coded for now
					};

					WriteDescSets.push_back(
						VkWriteDescriptorSet{
							.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
							.dstSet = m_DescSets[i],
							.dstBinding = mBindings[j].mBinding,
							.dstArrayElement = 0,
							.descriptorCount = mBindings[j].mDescCount,
							.descriptorType = mBindings[j].mDescType,
							.pImageInfo = &ImageInfo,
						}
						);
				}

				// # Update the descriptor sets
				vkUpdateDescriptorSets(
					GetCurrentContext()->getDeviceRes().pDevice->getHandle(), (uint32_t)WriteDescSets.size(),
					WriteDescSets.data(), 0, NULL
				);
			}
		}
	}
}