
/* # Editor entry point */

//#include <Editor.h>
#include <Engine.h>

int main(int argc, char* argv[])
{
	std::string path_only_for_development = PROJ_PATH;

	// # Application info
	Engine::EngineInfo mAppInfo = {
		.mProjName = "development_test_project",
		.mProjPath = path_only_for_development
	};
	// # Create the engine
	Engine::EngineCore* pEngine = Engine::Create_Engine(mAppInfo);

	//// # Create the editor
	//Editor::Editor mEditor;
	//mEditor.Initialize(pEngine);

	pEngine->Run();
	pEngine->Destroy();
}
