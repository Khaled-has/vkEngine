
# == ImGui SDL3
add_library(ImGui_ImplSDL3)

target_sources(ImGui_ImplSDL3
	PRIVATE
		"imgui_impl_sdl3.cpp" 
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"imgui_impl_sdl3.h"
)

target_include_directories(ImGui_ImplSDL3
	PUBLIC
		"${EXTERN_DIR}/SDL3/include"
)

# == ImGui vulkan
add_library(ImGui_ImplVulkan)

target_sources(ImGui_ImplVulkan
	PRIVATE
		"imgui_impl_vulkan.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"imgui_impl_vulkan.h"
)

target_include_directories(ImGui_ImplVulkan
	PUBLIC
		"${EXTERN_DIR}/vulkan/include"
		"${EXTERN_DIR}/volk"
)

# == ImGui UI
add_library(ImGui_UI)

target_sources(ImGui_UI
	PRIVATE
		"imgui.cpp" "imgui_draw.cpp" "imgui_demo.cpp"
		"imgui_tables.cpp" "imgui_widgets.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"imconfig.h" "imgui.h" "imgui_internal.h"
)