
#include "Engine.h"
#include "Application.h"

namespace Engine
{
	EngineCore* Create_Engine(ApplicationInfo mInfo)
	{
		return new Application(mInfo);
	}
}