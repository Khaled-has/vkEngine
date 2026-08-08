
#include "Engine.h"
#include "Application.h"

namespace Engine
{
	void  EngineCore::SetCurrent(EngineCore* pCurrent)
	{
		pInstance = (Application*)pCurrent;
	}

	EngineCore* Create_Engine(EngineInfo mInfo)
	{
		return new Application(mInfo);
	}
}