#include "VK_Buffer.h"

#include "config.h"
#include "VK_Device.h"

void VK_Buffer::Create(VkBufferCreateInfo mBufferInfo, VmaAllocationCreateInfo mAllocInfo)
{
	CHECK_VK_RES(
		vmaCreateBuffer(VK_Device::Get()->getAllocator(), &mBufferInfo, &mAllocInfo, &m_pBuffer, &m_pAllocation, &m_pAllocInfo),
		"vmaCreateBuffer"
	);
}

void VK_Buffer::Destroy()
{
	vmaDestroyBuffer(VK_Device::Get()->getAllocator(), m_pBuffer, m_pAllocation);
}
