
add_library(vkRenderer)

target_sources(vkRenderer
	PRIVATE
		"Renderer.cpp" "vkRenderer.cpp" "RenderGraph.cpp" "imgui/ImGuiRenderer.cpp" "DrawColorPass.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Renderer.h"
)

# == include
target_link_libraries(vkRenderer 
	PRIVATE
		vkGPU
		vkPlatforms
		ImGui_Core
)