
add_library(volk)

target_sources(volk
	PRIVATE
		"volk.c"
	PUBLIC 
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"volk.h"
)

target_include_directories(volk PUBLIC "${EXTERN_DIR}/vulkan/include")