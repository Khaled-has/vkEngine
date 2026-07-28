#pragma once

#include <stdint.h>
#include <imgui.h>

namespace UI
{
	struct TexInfo
	{
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mChannels;
	};

	class Texture
	{
	public:
		Texture() = default;

		void load(const char* pFileName);
		void destroy();

		inline TexInfo getTexSize() { return mTexInfo; }
		operator ImTextureID() { return mImID; }
		
	private:
		ImTextureID mImID;
		TexInfo mTexInfo;

		// # VK_Texture
		void* mTexture;
	};

}