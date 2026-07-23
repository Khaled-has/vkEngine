
add_library(ImGui_Core)

target_sources(ImGui_Core 
	PRIVATE
		"imgui.cpp" "imgui_draw.cpp" "imgui_demo.cpp" "imgui_impl_sdl3.cpp" 
		"imgui_impl_vulkan.cpp" "imgui_tables.cpp" "imgui_widgets.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"imconfig.h" "imgui.h" "imgui_internal.h" "imgui_impl_sdl3.h" "imgui_impl_vulkan.h"
)

target_include_directories(ImGui_Core
	PUBLIC
		"${EXTERN_DIR}/SDL3/include"
		"${EXTERN_DIR}/vulkan/include"
		"${EXTERN_DIR}/volk"
)

add_library(ImGui_Interface INTERFACE)

target_sources(ImGui_Interface
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"imconfig.h" "imgui.h" "imgui_internal.h"
)