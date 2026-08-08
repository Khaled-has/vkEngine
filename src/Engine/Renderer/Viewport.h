#pragma once

#include <stdint.h>
#include <Scene.h>
#include <Entity.h>

#include <imgui.h>

namespace Render
{
	struct ViewSize
	{
		uint32_t mWidth;
		uint32_t mHeight;
	};

	struct ViewportInfo
	{
		ViewSize mViewSize;
		const Engine::Scene* pScene;
		const Engine::Entity* pCamEntity;
	};

	class Viewport
	{
	public:
		Viewport() {}

		virtual void Create(ViewportInfo mInfo) = 0;
		virtual void Destroy() = 0;

		virtual ImTextureID getImageView() = 0;

	protected:
		ViewportInfo m_Info;
	};

	Viewport* Create_Viewport();
}