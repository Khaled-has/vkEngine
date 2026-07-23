#include "VK_GraphPipeline.h"

#include "config.h"
#include "VK_Device.h"

void VK_GraphPipeline::Create(uint32_t mImageCount, std::span<VK_Binding> mBindings)
{
	CreateDescriptorPool(mImageCount, mBindings);
	CreateDescriptorLayout(mImageCount, mBindings);
	AllocateDescriptorSets(mImageCount);
	UpdateDescriptorSets(mImageCount, mBindings);
}

void VK_GraphPipeline::Destroy()
{
	const VkDevice& Device = VK_Device::Get()->getDevice();

	vkDestroyDescriptorSetLayout(Device, m_pDescSetLayout, NULL);
	vkDestroyDescriptorPool(Device, m_pDescPool, NULL);

	vkDestroyPipeline(Device, m_pPipeline, NULL);
	vkDestroyPipelineLayout(Device, m_pPipLayout, NULL);
}

void VK_GraphPipeline::CreatePipeline(const VK_Shader& pShader, std::span<VK_Attachment> mAttachments, VkViewport mViewport, VkRect2D mScissor)
{
	VkShaderModule Vs = pShader.getVertexShader();
	VkShaderModule Fs = pShader.getFragmentShader();

	// # Create the pipeline
	VkPipelineShaderStageCreateInfo ShaderStagesCreateInfo[2] = {
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = Vs,
		.pName = "main"
	},
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = Fs,
		.pName = "main"
	} };

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
		.pViewports = &mViewport,
		.scissorCount = 1,
		.pScissors = &mScissor
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
	bool IsHaveDepthAttach = false;
	for (uint32_t i = 0; i < mAttachments.size(); i++)
	{
		if (mAttachments[i].mIsColorAttach)
		{
			ColorFormats.push_back(mAttachments[i].mFormat);
		}
		else if (mAttachments[i].mIsDepthAttach && IsHaveDepthAttach)
		{
			LOG_WARN("You have tow depth attachment should you have only one: We use the first DepthAttachment");
		}
		else if (mAttachments[i].mIsDepthAttach)
		{
			IsHaveDepthAttach = true;
			DepthFormat = mAttachments[i].mFormat;
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
		.pSetLayouts = &m_pDescSetLayout
	};

	const VkDevice& pDevice = VK_Device::Get()->getDevice();

	CHECK_VK_RES(vkCreatePipelineLayout(pDevice, &LayoutInfo, NULL, &m_pPipLayout), "vkCreatePipelineLayout");

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
		.layout = m_pPipLayout,
		.renderPass = nullptr,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	CHECK_VK_RES(vkCreateGraphicsPipelines(pDevice, VK_NULL_HANDLE, 1, &GPipelineInfo, NULL, &m_pPipeline), "vkCreateGraphicsPipelines");
}

void VK_GraphPipeline::Bind(uint32_t mIndex, const VK_CmdBuf& mCmdBuf)
{
	// 1 # Bind pipeline
	vkCmdBindPipeline(
		mCmdBuf.getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipeline
	);

	// 2 # Bind descriptor sets
	vkCmdBindDescriptorSets(
		mCmdBuf.getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipLayout,
		0, 1,
		&m_pDescSets[mIndex], 0, NULL
	);
}

void VK_GraphPipeline::CreateDescriptorPool(uint32_t mImageCount, std::span<VK_Binding> mBindings)
{
	std::vector<VkDescriptorPoolSize> PoolSizes;

	for (uint32_t i = 0; i < mBindings.size(); i++)
	{
		PoolSizes.push_back(
			VkDescriptorPoolSize{
				.type = mBindings[i].mType,
				.descriptorCount = mImageCount
			}
		);
	}

	VkDescriptorPoolCreateInfo PoolInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = mImageCount,
		.poolSizeCount = (uint32_t)PoolSizes.size(),
		.pPoolSizes = PoolSizes.data()
	};

	CHECK_VK_RES(vkCreateDescriptorPool(VK_Device::Get()->getDevice(), &PoolInfo, NULL, &m_pDescPool), "vkCreateDescriptorPool");
}

void VK_GraphPipeline::CreateDescriptorLayout(uint32_t mImageCount, std::span<VK_Binding> mBindings)
{
	std::vector<VkDescriptorSetLayoutBinding> Bindings;

	for (uint32_t i = 0; i < mBindings.size(); i++)
	{
		Bindings.push_back(
			VkDescriptorSetLayoutBinding{
				.binding = mBindings[i].mBind,
				.descriptorType = mBindings[i].mType,
				.descriptorCount = mBindings[i].mDescCount,
				.stageFlags = mBindings[i].mStage
			}
		);
	}

	VkDescriptorSetLayoutCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = (uint32_t)Bindings.size(),
		.pBindings = Bindings.data()
	};

	CHECK_VK_RES(vkCreateDescriptorSetLayout(VK_Device::Get()->getDevice(), &CreateInfo, NULL, &m_pDescSetLayout), "vkCreateDescriptorSetLayout");
}

void VK_GraphPipeline::AllocateDescriptorSets(uint32_t mImageCount)
{
	std::vector<VkDescriptorSetLayout> Layouts(mImageCount, m_pDescSetLayout);

	VkDescriptorSetAllocateInfo AllocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = m_pDescPool,
		.descriptorSetCount = mImageCount,
		.pSetLayouts = Layouts.data()
	};

	m_pDescSets.resize(mImageCount);
	
	CHECK_VK_RES(vkAllocateDescriptorSets(
		VK_Device::Get()->getDevice(), &AllocInfo, m_pDescSets.data()),
		"vkAllocateDescriptorSets"
	);
}

void VK_GraphPipeline::UpdateDescriptorSets(uint32_t mImageCount, std::span<VK_Binding> mBindings)
{
	for (uint32_t i = 0; i < mImageCount; i++)
	{
		std::vector<VkWriteDescriptorSet> WriteDescSets{};

		for (uint32_t j = 0; j < mBindings.size(); j++)
		{
			// # Buffers
			if (mBindings[j].mBuffer != nullptr && mBindings[j].mBufCount > 0)
			{
				VkDescriptorBufferInfo BufferInfo = {
					.buffer = mBindings[j].mBufCount > 1 ? mBindings[j].mBuffer[i].getBuffer() : mBindings[j].mBuffer->getBuffer(),
					.offset = 0,
					.range = VK_WHOLE_SIZE
				};

				WriteDescSets.push_back(
					VkWriteDescriptorSet{
						.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						.dstSet = m_pDescSets[i],
						.dstBinding = mBindings[j].mBind,
						.dstArrayElement = 0,
						.descriptorCount = mBindings[j].mDescCount,
						.descriptorType = mBindings[j].mType,
						.pBufferInfo = &BufferInfo,
					}
				);
			}
			// # Texture
			else if (mBindings[j].mTexture != nullptr && mBindings[j].mTexCount > 0)
			{
				const VK_Texture* CurTexture = mBindings[j].mTexCount > 1 ? &mBindings[j].mTexture[i] : mBindings[j].mTexture;

				VkDescriptorImageInfo ImageInfo = {
					.sampler = CurTexture->getSampler(),
					.imageView = CurTexture->getView(),
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL // TODO: Hard coded for now
				};

				WriteDescSets.push_back(
					VkWriteDescriptorSet{
						.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						.dstSet = m_pDescSets[i],
						.dstBinding = mBindings[j].mBind,
						.dstArrayElement = 0,
						.descriptorCount = mBindings[j].mDescCount,
						.descriptorType = mBindings[j].mType,
						.pImageInfo = &ImageInfo,
					}
				);
			}

			// # Update the descriptor sets
			vkUpdateDescriptorSets(
				VK_Device::Get()->getDevice(), (uint32_t)WriteDescSets.size(), 
				WriteDescSets.data(), 0, NULL
			);
		}
	}
}
