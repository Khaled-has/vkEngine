#include "ImGuiRenderer.h"

#include "Window.h"
#include <imgui_impl_vulkan.h>

#include "config.h"

static void CheckVkResult(VkResult res)
{
	if (res == 0)
	{
		return;
	}

	std::cout << "VK_ImGui: ImGui create info" << res << std::endl;

	if (res < 0)
	{
		abort();
	}
}

void ImGuiRenderer::Initialize(const VK_Device& mDevice, VK_SwapChain& mSwapChain, const VK_CmdPool& mCmdPool)
{
	ImGuiStyleInit();
	if (ImGui::GetCurrentContext() == nullptr)
	{
		std::cout << "Error Here\n";
		exit(1);
	}
	for (auto& Im : mSwapChain.getSwapChainTextures())
	{
		mImages.push_back(Im.getImage());
		mViews.push_back(Im.getView());
	}
	std::cout << ">> 3\n";
	mFormat = mSwapChain.getColorImageFormat();
	mRenderArea = mSwapChain.getScreenRect();

	// # Allocate command buffers
	mCmdBufs.resize(mSwapChain.getSwapChainImageCount());
	mCmdPool.AllocCmdBufs((uint32_t)mCmdBufs.size(), mCmdBufs.data());

	// # Create descriptor pool
	VkDescriptorPoolSize poolSizes[] = {
	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo poolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 1000 * (sizeof(poolSizes) / sizeof(VkDescriptorPoolSize)),
		.poolSizeCount = (uint32_t)(sizeof(poolSizes) / sizeof(VkDescriptorPoolSize)),
		.pPoolSizes = &poolSizes[0]
	};
	std::cout << ">> 4\n";
	CHECK_VK_RES(
		vkCreateDescriptorPool(
		mDevice.getDevice(),
		&poolCreateInfo, NULL,
		&mDescPool
		), 
		"vkCreateDescriptorPool"
	);
	std::cout << ">> 5\n";
	// # Initialize imgui
	VkFormat ColorFormat = mSwapChain.getColorImageFormat();

	ImGui_ImplVulkan_InitInfo InitInfo = {
		.ApiVersion = mDevice.getInstanceVersion(),
		.Instance = mDevice.getInstance(),
		.PhysicalDevice = mDevice.getSelectedPhysDevice().m_pPhysDev,
		.Device = mDevice.getDevice(),
		.QueueFamily = mDevice.getQueues().mGraphics->getQFamilyIndex(),
		.Queue = mDevice.getQueues().mGraphics->getHandle(),
		.DescriptorPool = mDescPool,
		.MinImageCount = mDevice.getSelectedPhysDevice().m_surfaceCaps.minImageCount,
		.ImageCount = mSwapChain.getSwapChainImageCount(),
		.PipelineInfoMain = {
		.RenderPass = NULL,
		.Subpass = 0,
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
		.PipelineRenderingCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			.pNext = NULL,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &ColorFormat,
			.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
			.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
		},
		},
		.UseDynamicRendering = true,
		.Allocator = NULL,
		.CheckVkResultFn = CheckVkResult
	};
	if (ImGui::GetCurrentContext() == nullptr)
	{
		std::cout << "Error Here\n";
		exit(1);
	}
	std::cout << ">> 6\n";
	ImGui_ImplVulkan_LoadFunctions(mDevice.getInstanceVersion(), [](const char* function_name, void* user_data) 
		{ 
			VkInstance instance = volkGetLoadedInstance();
			return vkGetInstanceProcAddr(instance, function_name); 
		}
	);
	std::cout << ">> 7\n";
	if (ImGui::GetCurrentContext() == nullptr)
	{
		std::cout << "Error Here\n";
		exit(1);
	}

	ImGui_ImplVulkan_Init(&InitInfo);
	std::cout << ">> 8\n";
}

const VK_CmdBuf& ImGuiRenderer::getCmdBuf(
	uint32_t mFrameIndex,
	std::function<void(const VkCommandBuffer&, VkImage, VkFormat, VkImageLayout, VkImageLayout, int)> FuncImageMemBarrier
)
{
	BeginCmdBuf(mCmdBufs[mFrameIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// Begin rendering
	BeginDynamicRendering(
		mCmdBufs[mFrameIndex].getHandle(), &mViews[mFrameIndex],
		1u, NULL,
		NULL, NULL, true, false, mRenderArea
	);

	ImDrawData* pDrawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(pDrawData, mCmdBufs[mFrameIndex].getHandle());

	// # End rendering
	vkCmdEndRendering(mCmdBufs[mFrameIndex].getHandle());

	FuncImageMemBarrier(
		mCmdBufs[mFrameIndex].getHandle(), mImages[mFrameIndex],
		mFormat,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1
	);

	vkEndCommandBuffer(mCmdBufs[mFrameIndex].getHandle());

	return mCmdBufs[mFrameIndex];
}

void ImGuiRenderer::Render()
{
	ImGui::Render();
}

void ImGuiRenderer::Destroy()
{
	ImGui_ImplVulkan_Shutdown();
}

void ImGuiRenderer::ImGuiStyleInit()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	std::cout << ">> 1\n";
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.DisplaySize.x = (float)Window::getInstance()->getWindowInfo().mWidth;
	io.DisplaySize.y = (float)Window::getInstance()->getWindowInfo().mHeight;

	// # Initialize imgui impl window for vulkan
	Window::getInstance()->ImGui_ImplWindow_Init();
	std::cout << ">> 2\n";
}
