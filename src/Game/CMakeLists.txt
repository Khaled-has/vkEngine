
add_library(vkGame SHARED)

target_sources(vkGame
	PRIVATE
		
)

target_link_libraries(vkGame
	PRIVATE
		vkCore
)

add_compile_definitions(vkGame