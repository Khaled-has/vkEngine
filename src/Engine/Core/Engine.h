#pragma once

#include <functional>

#include "EngineAPI.h"
#include "Layer.h"

namespace Engine
{
	extern "C" VK_ENGINE_API int Create_Engine(std::function<void(void*)> mEditorInitFunc_, int argc, char* argv[]);
}