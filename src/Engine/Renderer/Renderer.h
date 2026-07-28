#pragma once

#include "Viewport.h"

namespace Render
{
	class Renderer
	{
	public:
		Renderer() {}

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual void Rendering() = 0;

		virtual void AddViewport(const Viewport& mViewport) = 0;
	};

	Renderer* CreateRenderer();
}