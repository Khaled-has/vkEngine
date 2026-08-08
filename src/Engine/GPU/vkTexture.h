#ifndef VKTEXTURE_H
#define VKTEXTURE_H

#include "vkBuffer.h"

namespace GPU
{
	struct vkTextureSamplerInfo
	{
		VkFilter mMinFilter;
		VkFilter mMaxFilter;
		VkSamplerAddressMode mAddressMode;
	};

	struct vkTextureInfo
	{
		VkFormat mFormat;
		VkImageViewType mViewType;
		VkImageAspectFlags mAspect; 
		uint32_t mLayerCount = 1u;

	};

	class VKGPU_API vkTexture
	{
		friend class vkSwapChain;
	public:
		vkTexture() {}

		void Create(vkTextureInfo mInfo, vkTextureSamplerInfo mSamplerInfo);
		// # Create the image view only ( Use if you have the VkImage loaded already )
		void CreateViewOnly(VkImage mImage, vkTextureInfo mInfo, vkTextureSamplerInfo mSamplerInfo);

		inline operator VkImage()		const { return m_Image; }
		inline operator VkImageView()	const { return m_View; }
		inline operator VkSampler()		const { return m_Sampler; }
		inline operator VkImageLayout() const { return m_Layout; }
	private:
		VkImage m_Image		   = VK_NULL_HANDLE;
		VkImageView m_View	   = VK_NULL_HANDLE;
		VkSampler m_Sampler	   = VK_NULL_HANDLE;
		VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		vkTextureInfo m_Info;
		vkTextureSamplerInfo m_SamplerInfo;

		void CreateSampler(vkTextureSamplerInfo mInfo);

		bool m_Created = false;

		bool CheckLoad();
	};

	VKGPU_API void CopyBufferToImage(const vkBuffer& mSrcBuf, const vkTexture& mDstTex);
}

#endif