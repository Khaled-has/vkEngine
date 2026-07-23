#include "ITexture.h"

namespace UI
{
	void CreateTexture(const char* pFileName, Texture* pTexture)
	{
		pTexture = new ITexture(pFileName);
	}

	uint32_t Texture::ImTextureID()
	{
		return mID;
	}
}