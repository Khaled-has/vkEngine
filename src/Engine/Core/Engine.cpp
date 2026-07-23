
#include "Engine.h"
#include "Application.h"

namespace Engine
{
	int Create_Engine(std::function<void(void*)> mEditorInitFunc_, int argc, char* argv[])
	{
		ApplicationInfo info = {
			.mProjName = "vkEngine",
			.mProjPath = "NONE",
			.mEditorInitFunc = mEditorInitFunc_
		};

		auto* m_pApplication = new Application(info);
		delete m_pApplication;

		return 0;
	}
}