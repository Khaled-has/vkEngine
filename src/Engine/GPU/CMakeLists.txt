
add_library(vkGPU)

target_sources(vkGPU
	PRIVATE
		#"${EXTERN_DIR}/volk/volk.c"
		"VK_Device.cpp" "VK_SwapChain.cpp" "VK_Queue.cpp" "VK_Semaphore.cpp"
		"VK_CmdBuf.cpp" "VK_GraphPipeline.cpp" "VK_RenderPass.cpp" "VK_Shader.cpp"
		"VK_Buffer.cpp" "VK_Texture.cpp" 
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"vkGPU.h"
)

target_include_directories(vkGPU PUBLIC
	"${EXTERN_DIR}/vma"
	"${EXTERN_DIR}/shaderc"
)

target_link_libraries(vkGPU 
	PRIVATE
		vkPlatforms
		"${EXTERN_DIR}/shaderc/shaderc_shared.lib"
	PUBLIC
		vkCoreInterface
		volk
)