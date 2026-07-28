
add_library(vkAssetsManager)

target_sources(vkAssetsManager 
	PRIVATE
		"AssetsManager.cpp" "ConfigManager.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"AssetsManager.h" "ConfigManager.h"
)

target_include_directories(vkAssetsManager 
	PRIVATE
		"${EXTERN_DIR}/Json"
)