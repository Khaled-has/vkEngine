#include "Renderer.h"

#include "vkRenderer.h"

namespace Render
{
	void Renderer::SetCurrent(Renderer* pCurrent)
	{ 
		pInstance = pCurrent; 
	}

	Renderer* CreateRenderer()
	{
		return new vkRenderer();
	}
}