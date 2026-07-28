#pragma once

#include "Texture.h"

namespace UI
{
	struct UIContext
	{
		void* pvolkProcAddress;
		void* pDevice;
	};

	void CreateContext();

	void SetUIContext(UIContext* pUIContext);
	UIContext* GetUIContext();
}