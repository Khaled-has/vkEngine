#include "vkQueue.h"
#include "vkContext.h"

namespace GPU
{

	void vkQueue::CreateQueue(uint32_t mQFamilyIndex, uint32_t mQIndex)
	{
		m_QFamilyIndex = mQFamilyIndex;
		m_QueueIndex = mQIndex;
		// # Setup the queue
		vkGetDeviceQueue(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), mQFamilyIndex,
			mQIndex, &m_Queue
		);
	}

	void SubmitSync(const vkQueue& mQueue)
	{
		
	}

	void WaitIdle(const vkQueue& mQueue)
	{
		vkQueueWaitIdle(mQueue.getHandle());
	}

}