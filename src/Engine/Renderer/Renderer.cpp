#include "Renderer.h"

#include "vkRenderer.h"

Renderer* CreateRenderer()
{
	return new vkRenderer();
}