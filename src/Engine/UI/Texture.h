#pragma once

#include <stdint.h>

#include "EngineAPI.h"

namespace UI
{
	struct VK_ENGINE_API TexInfo
	{
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mChannels;
	};

	class VK_ENGINE_API Texture
	{
	public:
		Texture() {}

		virtual void destroy() = 0;

		inline TexInfo getTexSize() { return mTexInfo; }
		uint32_t ImTextureID();

	protected:
		virtual void load(const char* pFileName) = 0;
		uint32_t mID;
		TexInfo mTexInfo;
	};

	extern "C" VK_ENGINE_API void CreateTexture(const char* pFileName, Texture* pTexture);
}