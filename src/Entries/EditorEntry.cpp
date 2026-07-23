
/* # Editor entry point */

#include <Engine.h>

#include <Editor.h>

int main(int argc, char* argv[])
{
	// # Editor 
	auto _EditorInit = [](void* pImContex) {
		Editor::Editor editor;
		editor.Initialize(pImContex);
	};

	// # Create the engine
	return Engine::Create_Engine(
		_EditorInit,
		argc, argv
	);
}
