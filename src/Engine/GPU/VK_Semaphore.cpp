#include "VK_Semaphore.h"

#include "config.h"
#include "VK_Device.h"

VkSemaphore CreateVkSemaphore()
{
	VkSemaphoreCreateInfo CreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};

	VkSemaphore Semaphore;
	CHECK_VK_RES(vkCreateSemaphore(
		VK_Device::Get()->getDevice(), &CreateInfo,
		NULL, &Semaphore
	), "vkCreateSemaphore");

	return Semaphore;
}

void VK_SemaphoresManager::Create(uint32_t mFrameCount)
{
	mNumImages = mFrameCount;
	m_Semaphores.resize(mFrameCount);

	VkFenceCreateInfo fenceInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext =  NULL,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (auto& Sem : m_Semaphores)
	{
		Sem.mImageAvailableSem = CreateVkSemaphore();
		Sem.mRenderFinishedSem = CreateVkSemaphore();

		CHECK_VK_RES(vkCreateFence(
			VK_Device::Get()->getDevice(), &fenceInfo, NULL, &Sem.mInFlightFence
		), "vkCreateFence");
		Sem.mImageInFlightFence = VK_NULL_HANDLE;
	}
}

void VK_SemaphoresManager::Destroy()
{
	const VkDevice& device = VK_Device::Get()->getDevice();

	for (auto& Sem : m_Semaphores)
	{
		vkDestroySemaphore(device, Sem.mImageAvailableSem, NULL);
		vkDestroySemaphore(device, Sem.mRenderFinishedSem, NULL);

		vkDestroyFence(device, Sem.mInFlightFence, NULL);
	}
}

VkResult VK_SemaphoresManager::AcquireNextImage(const VK_SwapChain& mSwapChain)
{
	const VkDevice& Device = VK_Device::Get()->getDevice();
	const VkSwapchainKHR& pSwapChain = mSwapChain.getSwapchain();
	vkWaitForFences(Device, 1, &m_Semaphores[mFrameIndex].mInFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(Device, 1, &m_Semaphores[mFrameIndex].mInFlightFence);

	VkResult res = vkAcquireNextImageKHR(
		Device, pSwapChain,
		UINT64_MAX, m_Semaphores[mFrameIndex].mImageAvailableSem,
		VK_NULL_HANDLE, &mAcquiredImageIndex
	);

	if ((m_Semaphores[mAcquiredImageIndex].mImageInFlightFence != VK_NULL_HANDLE) &&
		(m_Semaphores[mAcquiredImageIndex].mImageInFlightFence != m_Semaphores[mFrameIndex].mInFlightFence))
	{
		vkWaitForFences(Device, 1, &m_Semaphores[mAcquiredImageIndex].mImageInFlightFence, VK_TRUE, UINT64_MAX);
	}

	m_Semaphores[mAcquiredImageIndex].mImageInFlightFence = m_Semaphores[mFrameIndex].mInFlightFence;

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
	{
		return res;
	}
	
	if (res != VK_SUCCESS)
	{
		LOG_ERROR("Error: vkAcquiredNextImage");
	}

	return res;
}

void VK_SemaphoresManager::SubmitAsync(const VK_CmdBuf* CmdBuf, uint32_t Count, const VK_Queue& mQueue)
{
	static VkPipelineStageFlags waitFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	std::vector<VkCommandBuffer> Cmds;
	for (uint32_t i = 0; i < Count; i++)
	{
		Cmds.push_back(CmdBuf[i].getHandle());
	}

	VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_Semaphores[mFrameIndex].mImageAvailableSem,
		.pWaitDstStageMask = waitFlags,
		.commandBufferCount = Count,
		.pCommandBuffers = Cmds.data(),
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_Semaphores[mFrameIndex].mRenderFinishedSem
	};

	CHECK_VK_RES(
		vkQueueSubmit(mQueue.getHandle(), 1, &SubmitInfo, m_Semaphores[mFrameIndex].mInFlightFence), 
		"vkQueueSubmit"
	);
}

VkResult VK_SemaphoresManager::Present(uint32_t mImageIndex, const VK_SwapChain& mSwapChain, const VK_Queue& mQueue)
{
	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_Semaphores[mFrameIndex].mRenderFinishedSem,
		.swapchainCount = 1,
		.pSwapchains = &mSwapChain.getSwapchain(),
		.pImageIndices = &mImageIndex,
		.pResults = NULL
	};

	VkResult res = VK_ERROR_OUT_OF_DATE_KHR;

	res = vkQueuePresentKHR(mQueue.getHandle(), &PresentInfo);

	mFrameIndex = (mFrameIndex + 1) % mNumImages;

	WaitIdle(mQueue.getHandle());

	return res;
}

void VK_SemaphoresManager::WaitIdle(const VkQueue& mQueue)
{
	CHECK_VK_RES(vkQueueWaitIdle(mQueue), "vkQueueWaitIdle");
}
