#ifndef VK_QUEUE_H
#define VK_QUEUE_H

#include <volk.h>

class VK_Queue
{
public:
	VK_Queue(uint32_t mQFamilyIndex, uint32_t mQueueIndex) 
		: m_pQFamilyIndex(mQFamilyIndex), m_pQIndex(mQueueIndex)
	{
	}

	void Initialize();

	void SubmitSync(const VkCommandBuffer* pCmdBufs, uint32_t mCmdCount) const;

	inline const VkQueue& getHandle() const { return m_pQueue; }
	uint32_t getQFamilyIndex() { return m_pQFamilyIndex; }
private:
	VkQueue m_pQueue = VK_NULL_HANDLE;

	uint32_t m_pQFamilyIndex, m_pQIndex;
	bool m_pIsCreated = false;
};

#endif