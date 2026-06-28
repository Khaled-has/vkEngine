#include "VK_Queue.h"

#include "config.h"
#include "VK_Device.h"

void VK_Queue::Initialize()
{
	if (m_pIsCreated)
	{
		return;
	}

	// # Setup the queue
	vkGetDeviceQueue(
		VK_Device::Get()->getDevice(), m_pQFamilyIndex, 
		m_pQIndex, &m_pQueue
	);

	m_pIsCreated = true;
}

void VK_Queue::SubmitSync(const VkCommandBuffer* pCmdBufs, uint32_t mCmdCount) const
{
	VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = VK_NULL_HANDLE,
		.pWaitDstStageMask = VK_NULL_HANDLE,
		.commandBufferCount = mCmdCount,
		.pCommandBuffers = pCmdBufs,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = VK_NULL_HANDLE
	};

	CHECK_VK_RES(vkQueueSubmit(m_pQueue, 1, &SubmitInfo, VK_NULL_HANDLE), "vkQueueSubmit");
}
