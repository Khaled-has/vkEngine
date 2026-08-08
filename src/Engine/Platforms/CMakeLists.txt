
add_library(vkPlatforms)

target_sources(vkPlatforms
	PRIVATE
		"Window.cpp" "Windows/WindowsWindow.cpp" "Windows/WindowsTime.cpp" "Windows/WIN32_CustumTBar_ImplSDL3.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Window.h"
)

# === SDL3
target_include_directories(vkPlatforms 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${EXTERN_DIR}/SDL3/include"
)

# == link
target_link_libraries(vkPlatforms
	PRIVATE
		vkCoreInterface
		ImGui_ImplSDL3
		ImGui_ImplVulkan
	PUBLIC
		vkEvents
)

if(WIN32)

	target_link_libraries(vkPlatforms
		PRIVATE
			dwmapi
			gdi32
			user32
			kernel32
	)

endif()