#pragma once

#include "Viewport.h"

namespace Render
{
	class Renderer
	{
	public:
		Renderer() {}

		static void SetCurrent(Renderer* pCurrent);

		static inline void AddViewport(Viewport* pViewport) { pInstance->AddViewportImpl(pViewport); }

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual void Rendering() = 0;

		virtual void AddViewportImpl(Viewport* pViewport) = 0;

	protected:
		static Renderer* pInstance;
	};

	Renderer* CreateRenderer();
}