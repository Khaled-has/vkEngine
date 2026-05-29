#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include "VK_Device.h"

class VK_TextureView
{
public:
	VK_TextureView(
		const VkImage& mImage, VkFormat mFormat,
		VkImageViewType mViewType, VkImageAspectFlags mAspectFlags, 
		uint32_t mLayerCount = 1
		);

	VK_TextureView() {}

	void CreateView(const VkImage& mImage, VkFormat mFormat,
		VkImageViewType mViewType, VkImageAspectFlags mAspectFlags,
		uint32_t mLayerCount = 1);
	void DestroyView();

	inline const VkImageView& getView() const { return m_pView; }

	inline VkExtent2D getExtent2D() const { return mExtent2D; }
	inline VkFormat getFormat() const { return mFormat; }
	inline uint32_t getLayerCount() const { return mLayerCount; }


protected:
	VkImageView m_pView = VK_NULL_HANDLE;
	VkExtent2D mExtent2D;
	VkFormat mFormat;
	uint32_t mLayerCount = 0;

	bool m_pIsCreated = false;
};

class VK_Texture : public VK_TextureView
{
public:
	VK_Texture() {}

	bool CreateImage(const void* pData, VkExtent2D mExtent2D, VkFormat mForamt, uint32_t mLayerCount);
	void DestroyImage();

	inline const VkImage& getImage() const { return m_pImage; }

private:
	VkImage m_pImage = VK_NULL_HANDLE;
	VmaAllocation m_pAllocation = VK_NULL_HANDLE;
};

#endif