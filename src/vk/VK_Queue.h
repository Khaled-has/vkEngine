#ifndef VK_QUEUE_H
#define VK_QUEUE_H

#include <volk.h>

class VK_Queue
{
public:
	VK_Queue(uint32_t mQueueIndex) 
		: m_pQIndex(mQueueIndex)
	{
		Initialize();
	}

private:
	VkQueue m_pQueue = VK_NULL_HANDLE;

	uint32_t m_pQIndex;

	void Initialize();
};

#endif