
add_library(vkAssetsManager)

target_sources(vkAssetsManager 
	PRIVATE
		"AssetsManager.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"AssetsManager.h"
)