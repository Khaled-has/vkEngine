#pragma once

#include <vkGPU.h>
#include "Texture.h"

namespace UI
{
	class VK_ENGINE_API ITexture : public Texture
	{
	public:
		ITexture() {}
		ITexture(const char* pFileName) { load(pFileName); }

		virtual void destroy() override
		{
			mTexture.DestroyImage();
		}

	protected:
		virtual void load(const char* pFileName) override;

	private:
		VK_Texture mTexture;
		VkDescriptorSet mDescSet = VK_NULL_HANDLE;
	};
}