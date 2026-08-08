# vkCoreInterface
add_library(vkCoreInterface)

target_sources(vkCoreInterface
	PRIVATE
		"Log.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Timer.h" "Layer.h" "EngineAPI.h" "Log.h"
)

target_link_libraries(vkCoreInterface 
	PUBLIC
		vkEvents
)

# == vkCore
add_library(vkCore SHARED)

add_compile_definitions(vkCore PRIVATE ENGINE_BUILD)

target_sources(vkCore
	PRIVATE
		 "${EXTERN_DIR}/stb/stb_image.cpp"	# stb_image
		 "Application.cpp" "LayerStack.cpp" "Layer.cpp" "Engine.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"EngineAPI.h" "Engine.h" "Layer.h"
)

target_link_libraries(vkCore
	PRIVATE
		# local
		vkPlatforms
		vkAssetsManager

		# == extern
		# = vkPlatform & vkGPU & vkRenderer
		ImGui_Core
		volk

		# = vkPlatform & ImGui_Core
		SDL3::SDL3

		# = vkPlatform
		dwmapi
		gdi32
		user32
		kernel32
	PUBLIC
		vkEvents
		vkScene
		vkRenderer
		vkUI
		ImGui_UI
)