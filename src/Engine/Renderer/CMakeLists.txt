
add_library(vkRenderer)

target_sources(vkRenderer
	PRIVATE
		"Renderer.cpp" "vkRenderer.cpp" "Viewport.cpp" "Viewport.cpp"
		"RenderGraph.cpp" "imgui/ImGuiRenderer.cpp" "DrawColorPass.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Renderer.h" "Viewport.h"
)

# == include
target_link_libraries(vkRenderer 
	PRIVATE
		vkGPU
		vkPlatforms
		vkScene
	PUBLIC
		ImGui_Core
)