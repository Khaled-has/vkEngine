#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include "VK_Buffer.h"

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
	void CreateSampler(VkFilter mMinFilter, VkFilter mMaxFilter, VkSamplerAddressMode mAddressMode);

	void DestroyView();
	void DestroySampler();

	inline VK_TextureView* getTextureView() { return this; }

	inline void setLayout(VkImageLayout mLayout) { this->mLayout = mLayout; }

	inline const VkImageView& getView() const { return m_pView; }
	inline const VkSampler& getSampler() const { return m_pSampler; }

	inline VkExtent3D getExtent3D() const { return mExtent3D; }
	inline VkFormat getFormat() const { return mFormat; }
	inline VkImageLayout getLayout() const { return mLayout; }
	inline uint32_t getLayerCount() const { return mLayerCount; }

protected:
	VkImageView m_pView = VK_NULL_HANDLE;
	VkSampler m_pSampler = VK_NULL_HANDLE;
	VkExtent3D mExtent3D;
	VkFormat mFormat;
	VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	uint32_t mLayerCount = 0;

	bool m_pIsCreated = false;
};

class VK_Texture : public VK_TextureView
{
public:
	VK_Texture() {}
	inline VK_Texture(VkImage& mImage, VkFormat mFormat) { m_pImage = mImage; this->mFormat = mFormat; }

	void CreateImage(VkExtent3D mExtent, VkFormat mFormat, uint32_t mLayerCount);
	void DestroyImage();

	inline const VkImage& getImage() const { return m_pImage; }

private:
	VkImage m_pImage = VK_NULL_HANDLE;
	VmaAllocation m_pAllocation = VK_NULL_HANDLE;
};

void CopyBufferToImage(const VK_Buffer& mBuffer, VK_Texture& mImage, std::span<VkBufferImageCopy> pRegions);

#endif