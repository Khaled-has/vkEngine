
add_library(vkEvents INTERFACE)

target_sources(vkEvents 
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Event.h" "KeyEvent.h" "Code.h" "ApplicationEvent.h"
)

set_target_properties(vkEvents PROPERTIES LINKER_LANGUAGE CXX)