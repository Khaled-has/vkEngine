#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <volk.h>
#include <vk_mem_alloc.h>

class VK_Buffer
{
public:
	VK_Buffer() {}

	void Create(VkBufferCreateInfo mBufferInfo, VmaAllocationCreateInfo mAllocInfo);
	void Destroy();

	inline const VkBuffer& getBuffer() const { return m_pBuffer; }
	inline const VmaAllocation& getAllocation() const { return m_pAllocation; }
	inline const VmaAllocationInfo& getAllocInfo() const { return m_pAllocInfo; }

	inline VkDeviceSize getBufferSize() const { return m_pBufferSize; }

private:
	VkBuffer m_pBuffer = VK_NULL_HANDLE;
	VmaAllocation m_pAllocation = VK_NULL_HANDLE;
	VmaAllocationInfo m_pAllocInfo;
	VkDeviceSize m_pBufferSize = 0;
};

#endif