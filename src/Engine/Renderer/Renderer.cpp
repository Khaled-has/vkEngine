#include "Renderer.h"

#include "vkRenderer.h"

namespace Render
{
	Renderer* CreateRenderer()
	{
		return new vkRenderer();
	}
}